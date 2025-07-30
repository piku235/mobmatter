#include "AppConfig.h"
#include "CHIPProjectAppConfig.h"
#include "SqliteDatabaseSchema.h"
#include "application/subscribers/ChipCoverEndpointSubscriber.h"
#include "application/subscribers/MobilusCoverControlSubscriber.h"
#include "common/domain/DomainEventPublisher.h"
#include "common/logging/LogHandler.h"
#include "common/logging/Logger.h"
#include "common/logging/handlers/StdioLogHandler.h"
#include "common/logging/handlers/SyslogHandler.h"
#include "common/persistence/sqlite/Connection.h"
#include "driven_adapters/logging/LoggingDomainEventSubscriber.h"
#include "driven_adapters/matter/window_covering_reporting/WindowCoveringReportingAdapter.h"
#include "driven_adapters/matter/zcl_cover_endpoint/ZclCoverEndpointService.h"
#include "driven_adapters/mobilus/cover_control/MqttMobilusCoverControlService.h"
#include "driven_adapters/persistence/sqlite/SqliteCoverRepository.h"
#include "driven_adapters/persistence/sqlite/SqliteEndpointIdGenerator.h"
#include "driving_adapters/matter/cluster_stubs/ClusterStubsAdapter.h"
#include "driving_adapters/matter/window_covering_cluster/WindowCoveringClusterAdapter.h"
#include "driving_adapters/mobilus/device_events/MobilusCoverEventHandler.h"
#include "driving_adapters/mobilus/device_events/MqttMobilusDeviceStateSyncer.h"
#include "driving_adapters/mobilus/device_events/MqttMobilusEventSubscriber.h"
#include "driving_adapters/mobilus/device_init/MobilusCoverInitHandler.h"
#include "driving_adapters/mobilus/device_init/MqttMobilusDeviceInitializer.h"
#include "jungi/mobilus_gtw_client/MqttMobilusGtwClient.h"
#include "jungi/mobilus_gtw_client/proto/DeviceSettingsRequest.pb.h"
#include "matter/ChipAppMain.h"
#include "matter/event_loop/DomainEventPublisherAdapter.h"
#include "matter/event_loop/MqttMobilusGtwClientAdapter.h"
#include "matter/persistence/SqlitePersistentStorageDelegate.h"

#include <platform/CHIPDeviceLayer.h>

#include <cstdio>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <memory>
#include <optional>
#include <string>

#ifdef CONFIG_INI_FILE
#include <inipp/inipp.h>
#endif

using namespace mmbridge::common::persistence;
using namespace mmbridge::common::domain;
using namespace mmbridge::common::logging;
using namespace mmbridge::common::logging::handlers;
using namespace mmbridge::application;
using namespace mmbridge::application::subscribers;
using namespace mmbridge::application::driven_ports;
using namespace mmbridge::driven_adapters::persistence::sqlite;
using namespace mmbridge::driven_adapters::mobilus::cover_control;
using namespace mmbridge::driven_adapters::matter::zcl_cover_endpoint;
using namespace mmbridge::driven_adapters::matter::window_covering_reporting;
using namespace mmbridge::driven_adapters::logging;
using namespace mmbridge::driving_adapters::mobilus::device_events;
using namespace mmbridge::driving_adapters::mobilus::device_init;
using namespace mmbridge::driving_adapters::matter::window_covering_cluster;
using namespace mmbridge::driving_adapters::matter::cluster_stubs;
using namespace mmbridge::matter;
using namespace mmbridge::matter::event_loop;
using namespace mmbridge::matter::persistence;
using namespace jungi::mobilus_gtw_client;
using mmbridge::application::model::MobilusDeviceId;

static ChipAppMain sChipApp;

void handleSignal(int signal)
{
    sChipApp.shutdown();
}

const char* getEnvOr(const char* name, const char* defaultValue)
{
    const char* value = getenv(name);

    return value ? value : defaultValue;
}

#ifdef CONFIG_INI_FILE
void parseIniConfig(AppConfig& config)
{
    std::ifstream fs(CONFIG_INI_FILE);
    if (!fs) {
        return;
    }

    inipp::Ini<char> ini;
    ini.parse(fs);

    auto sit = ini.sections.find("mobilus");
    if (ini.sections.end() == sit) {
        return;
    }

    auto& section = sit->second;

    if (!section["username"].empty()) {
        config.mobilusUsername = section["username"];
    }
    if (!section["password"].empty()) {
        config.mobilusPassword = section["password"];
    }
}
#endif

AppConfig loadAppConfig()
{
    AppConfig config = {
        .mobilusUsername = "admin",
        .mobilusPassword = "admin"
    };

#ifdef CONFIG_INI_FILE
    parseIniConfig(config);
#endif

    const char* value;

    if (nullptr != (value = getenv("MOBILUS_USERNAME"))) {
        config.mobilusUsername = value;
    }

    if (nullptr != (value = getenv("MOBILUS_PASSWORD"))) {
        config.mobilusPassword = value;
    }

    return config;
}

std::unique_ptr<MqttMobilusGtwClient> createMobilusGtwClient(const AppConfig& appConfig, MqttMobilusGtwClientAdapter* clientAdapter)
{
    MqttMobilusGtwClientConfig clientConfig(MOBILUS_HOST, MOBILUS_PORT, appConfig.mobilusUsername, appConfig.mobilusPassword, MOBILUS_CA_FILE);

    clientConfig.keepAliveMessage = std::make_unique<proto::DeviceSettingsRequest>();
    clientConfig.clientWatcher = clientAdapter;

    return MqttMobilusGtwClient::with(std::move(clientConfig));
}

std::optional<sqlite::Connection> openSqliteDb(Logger& logger)
{
    bool initiate = !std::filesystem::exists(DATABASE_FILE);
    auto db = sqlite::Connection::open(DATABASE_FILE, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);

    if (!db) {
        logger.critical("Failed to open database");
        return std::nullopt;
    }

    if (-1 == db->exec(DATABASE_PRAGMA_SQL)) {
        db.reset(); // close by destruct
        std::filesystem::remove(DATABASE_FILE);

        logger.critical("Failed to setup database connection");
        return std::nullopt;
    }

    if (initiate && -1 == db->exec(kDatabaseSchema)) {
        db.reset(); // close by destruct
        std::filesystem::remove(DATABASE_FILE);

        logger.critical("Failed to initialize database");
        return std::nullopt;
    }

    return db;
}

std::unique_ptr<LogHandler> createLogHandler()
{
    auto handler = getEnvOr("LOG", "stdio");

    if (!strcmp("syslog", handler)) {
        return std::make_unique<SyslogHandler>(LOG_IDENT);
    }

    return std::make_unique<StdioLogHandler>();
}

std::optional<MobilusDeviceId> getTestDeviceIdEnv()
{
    auto env = getenv("TEST_DEVICE_ID");

    if (!env) {
        return std::nullopt;
    }

    return static_cast<MobilusDeviceId>(atoi(env));
}

bool initDevices(MqttMobilusGtwClient& mobilusGtwClient, CoverRepository& coverRepository, EndpointIdGenerator& endpointIdGenerator, Logger& logger)
{
    MqttMobilusDeviceInitializer mobilusDeviceInitializer(mobilusGtwClient, logger);
    auto testDeviceId = getTestDeviceIdEnv();

    if (testDeviceId) {
        mobilusDeviceInitializer.useTestDeviceOnly(*testDeviceId);
    }

    mobilusDeviceInitializer.registerHandler(std::make_unique<MobilusCoverInitHandler>(coverRepository, endpointIdGenerator, logger));

    if (!mobilusDeviceInitializer.run()) {
        logger.critical("Failed to initialize devices");
        return false;
    }

    return true;
}

size_t loadDevices(CoverRepository& coverRepository, CoverEndpointService& coverEndpointService, Logger& logger)
{
    auto covers = coverRepository.all();

    for (auto& cover : covers) {
        coverEndpointService.addEndpoint(cover.endpointId(), cover.specification());
        logger.notice("Loaded cover at endpoint: %u", cover.endpointId());
    }

    return covers.size();
}

void showVersion()
{
    printf("matter-bridge %s\n", CHIP_DEVICE_CONFIG_DEVICE_SOFTWARE_VERSION_STRING);
}

int main(int argc, char* argv[])
{
    if (argc > 1 && !strcmp("--version", argv[1])) {
        showVersion();
        return 0;
    }

    // must outlive the main()
    static Logger logger(createLogHandler());
    static auto db = openSqliteDb(logger);

    if (!db) {
        return 1;
    }

    auto appConfig = loadAppConfig();
    auto& chipSystemLayer = static_cast<chip::System::LayerSocketsLoop&>(chip::DeviceLayer::SystemLayer());
    MqttMobilusGtwClientAdapter mobilusGtwClientAdapter(chipSystemLayer);
    auto mobilusGtwClient = createMobilusGtwClient(appConfig, &mobilusGtwClientAdapter);

    // chip
    static SqlitePersistentStorageDelegate persistentStorageDelegate;
    persistentStorageDelegate.Init(&*db);

    // driven
    SqliteCoverRepository coverRepository(*db);
    SqliteEndpointIdGenerator endpointIdGenerator(ZCL_INITIAL_DYNAMIC_ENDPOINT_ID, *db);
    ZclCoverEndpointService coverEndpointService(ZCL_AGGREGATOR_ENDPOINT_ID);
    MqttMobilusCoverControlService coverControlService(*mobilusGtwClient, logger);
    LoggingDomainEventSubscriber loggingDomainEventSubscriber(logger);

    // app subscribers
    auto& domainEventPublisher = DomainEventPublisher::instance();
    DomainEventPublisherAdapter domainEventPublisherAdapter(chipSystemLayer);
    MobilusCoverControlSubscriber mobilusCoverControlSubscriber(coverControlService);
    ChipCoverEndpointSubscriber chipCoverEndpointSubscriber(coverEndpointService);
    WindowCoveringReportingAdapter windowCoveringReportingAdapter;

    // driving
    MobilusCoverEventHandler mobilusCoverEventHandler(coverRepository, logger);
    MqttMobilusEventSubscriber mobilusEventSubscriber(*mobilusGtwClient, mobilusCoverEventHandler);
    MqttMobilusDeviceStateSyncer mobilusDeviceStateSyncer(*mobilusGtwClient, mobilusCoverEventHandler, logger);
    WindowCoveringClusterAdapter windowCoveringClusterAdapter(coverRepository, logger);
    ClusterStubsAdapter clusterStubsAdapter;

    signal(SIGINT, handleSignal);
    signal(SIGTERM, handleSignal);

    domainEventPublisher.subscribe(mobilusCoverControlSubscriber);
    domainEventPublisher.subscribe(chipCoverEndpointSubscriber);
    domainEventPublisher.subscribe(windowCoveringReportingAdapter);
    domainEventPublisher.subscribe(loggingDomainEventSubscriber);

    sChipApp.registerComponent(mobilusGtwClientAdapter);
    sChipApp.registerComponent(domainEventPublisherAdapter);
    sChipApp.registerComponent(windowCoveringClusterAdapter);
    sChipApp.registerComponent(clusterStubsAdapter);
    sChipApp.registerComponent(mobilusEventSubscriber);
    sChipApp.registerComponent(mobilusDeviceStateSyncer);

    int rc = sChipApp.boot(logger, *mobilusGtwClient, persistentStorageDelegate);
    if (rc) {
        return rc;
    }

    // after ZAP boot
    if (!loadDevices(coverRepository, coverEndpointService, logger)
        && !initDevices(*mobilusGtwClient, coverRepository, endpointIdGenerator, logger)) {
        return 1;
    }

    sChipApp.run();

    return 0;
}
