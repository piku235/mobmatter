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
#include "driven_adapters/matter/reporting/CoverReportingAdapter.h"
#include "driven_adapters/matter/zcl/ZclCoverEndpointService.h"
#include "driven_adapters/mobilus/MqttMobilusCoverControlService.h"
#include "driven_adapters/persistence/sqlite/SqliteCoverRepository.h"
#include "driven_adapters/persistence/sqlite/SqliteEndpointIdGenerator.h"
#include "driving_adapters/matter/cluster_stubs/ClusterStubsAdapter.h"
#include "driving_adapters/matter/cover_cluster/CoverClusterAdapter.h"
#include "driving_adapters/mobilus/MqttMobilusDeviceInitializer.h"
#include "driving_adapters/mobilus/MqttMobilusDeviceStateSyncer.h"
#include "driving_adapters/mobilus/MqttMobilusEventSubscriber.h"
#include "driving_adapters/mobilus/cover/MobilusCoverEventHandler.h"
#include "driving_adapters/mobilus/cover/MobilusCoverInitHandler.h"
#include "jungi/mobilus_gtw_client/MqttMobilusGtwClient.h"
#include "jungi/mobilus_gtw_client/proto/DeviceSettingsRequest.pb.h"
#include "matter/ChipAppMain.h"
#include "matter/event_loop/DomainEventPublisherAdapter.h"
#include "matter/event_loop/MobilusGtwEventLoopAdapter.h"
#include "matter/persistence/SqlitePersistentStorageDelegate.h"

#include <platform/CHIPDeviceLayer.h>

#include <csignal>
#include <cstdio>
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
using namespace mmbridge::driven_adapters::mobilus;
using namespace mmbridge::driven_adapters::matter::zcl;
using namespace mmbridge::driven_adapters::matter::reporting;
using namespace mmbridge::driven_adapters::logging;
using namespace mmbridge::driving_adapters::mobilus;
using namespace mmbridge::driving_adapters::mobilus::cover;
using namespace mmbridge::driving_adapters::matter::cover_cluster;
using namespace mmbridge::driving_adapters::matter::cluster_stubs;
using namespace mmbridge::matter;
using namespace mmbridge::matter::event_loop;
using namespace mmbridge::matter::persistence;
using mmbridge::application::model::MobilusDeviceId;
namespace mobgtw = jungi::mobilus_gtw_client;

static ChipAppMain sChipApp;

class MqttMobilusGtwClientLoggerAdapter : public mobgtw::logging::Logger {
public:
    // clang-format off
    MqttMobilusGtwClientLoggerAdapter(::Logger& logger): mLogger(logger) {}
    // clang-format on

    void info(const std::string& message) override { mLogger.info(kFormat, message.c_str()); }
    void error(const std::string& message) override { mLogger.error(kFormat, message.c_str()); }

private:
    static constexpr char kFormat[] = "GTW: %s";
    ::Logger& mLogger;
};

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

std::unique_ptr<mobgtw::MqttMobilusGtwClient> createMobilusGtwClient(const AppConfig& appConfig, mobgtw::io::EventLoop* loop, mobgtw::logging::Logger* logger)
{
    return mobgtw::MqttMobilusGtwClient::builder()
        .dsn({ true, std::nullopt, std::nullopt, MOBILUS_HOST, MOBILUS_PORT, MOBILUS_CA_FILE })
        .login({ appConfig.mobilusUsername, appConfig.mobilusPassword })
        .useKeepAliveMessage(std::make_unique<proto::DeviceSettingsRequest>())
        .useLogger(logger)
        .attachTo(loop)
        .build();
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

bool setupDevices(mobgtw::MqttMobilusGtwClient& mobilusGtwClient, CoverRepository& coverRepository, CoverEndpointService& coverEndpointService, EndpointIdGenerator& endpointIdGenerator, Logger& logger)
{
    auto covers = coverRepository.all();

    for (auto& cover : covers) {
        coverEndpointService.addEndpoint(cover.endpointId(), cover.specification());
        logger.notice("Loaded cover at endpoint: %u", cover.endpointId());
    }

    if (covers.size()) {
        return true;
    }

    // initiate if there are no persisted devices
    MqttMobilusDeviceInitializer mobilusDeviceInitializer(mobilusGtwClient, logger);
    mobilusDeviceInitializer.registerHandler(std::make_unique<MobilusCoverInitHandler>(coverRepository, endpointIdGenerator, logger));

    auto testDeviceId = getTestDeviceIdEnv();
    if (testDeviceId) {
        mobilusDeviceInitializer.useTestDeviceOnly(*testDeviceId);
    }

    if (!mobilusDeviceInitializer.run()) {
        logger.critical("Failed to initiate devices");
        return false;
    }

    return true;
}

void syncDeviceStates(mobgtw::MqttMobilusGtwClient& mobilusGtwClient, MobilusCoverEventHandler& mobilusCoverEventHandler, Logger& logger)
{
    MqttMobilusDeviceStateSyncer mobilusDeviceStateSyncer(mobilusGtwClient, mobilusCoverEventHandler, logger);
    mobilusDeviceStateSyncer.run();
}

void showVersion()
{
    printf("matter-bridge v%s\n", CHIP_DEVICE_CONFIG_DEVICE_SOFTWARE_VERSION_STRING);
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
    MobilusGtwEventLoopAdapter mobilusGtwEventLoopAdapter(chipSystemLayer);
    MqttMobilusGtwClientLoggerAdapter mobilusLoggerAdapter(logger);
    auto mobilusGtwClient = createMobilusGtwClient(appConfig, &mobilusGtwEventLoopAdapter, &mobilusLoggerAdapter);

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
    CoverReportingAdapter coverReportingAdapter;

    // driving
    MobilusCoverEventHandler mobilusCoverEventHandler(coverRepository, logger);
    MqttMobilusEventSubscriber mobilusEventSubscriber(*mobilusGtwClient, mobilusCoverEventHandler);
    CoverClusterAdapter coverClusterAdapter(coverRepository, logger);
    ClusterStubsAdapter clusterStubsAdapter;

    signal(SIGINT, handleSignal);
    signal(SIGTERM, handleSignal);

    domainEventPublisher.subscribe(mobilusCoverControlSubscriber);
    domainEventPublisher.subscribe(chipCoverEndpointSubscriber);
    domainEventPublisher.subscribe(coverReportingAdapter);
    domainEventPublisher.subscribe(loggingDomainEventSubscriber);

    sChipApp.registerComponent(mobilusGtwEventLoopAdapter);
    sChipApp.registerComponent(domainEventPublisherAdapter);
    sChipApp.registerComponent(coverClusterAdapter);
    sChipApp.registerComponent(clusterStubsAdapter);
    sChipApp.registerComponent(mobilusEventSubscriber);

    int rc = sChipApp.boot(logger, *mobilusGtwClient, persistentStorageDelegate);
    if (rc) {
        return rc;
    }

    // after ZAP boot
    if (!setupDevices(*mobilusGtwClient, coverRepository, coverEndpointService, endpointIdGenerator, logger)) {
        return 1;
    }

    syncDeviceStates(*mobilusGtwClient, mobilusCoverEventHandler, logger);

    sChipApp.run();

    return 0;
}
