#include "AppConfig.h"
#include "CHIPProjectAppConfig.h"
#include "SqliteDatabaseSchema.h"
#include "application/subscribers/CoverControlSubscriber.h"
#include "application/subscribers/CoverEndpointSubscriber.h"
#include "application/subscribers/DeviceManagementSubscriber.h"
#include "common/domain/DomainEventPublisher.h"
#include "common/logging/LogHandler.h"
#include "common/logging/Logger.h"
#include "common/logging/handlers/StdioLogHandler.h"
#include "common/logging/handlers/SyslogHandler.h"
#include "common/persistence/sqlite/Connection.h"
#include "driven_adapters/matter/reporting/CoverReportingAdapter.h"
#include "driven_adapters/matter/zcl/ZclCoverEndpointService.h"
#include "driven_adapters/mobilus/MqttMobilusCoverControlService.h"
#include "driven_adapters/mobilus/MqttMobilusDeviceManagementService.h"
#include "driven_adapters/persistence/sqlite/SqliteCoverRepository.h"
#include "driven_adapters/persistence/sqlite/SqliteEndpointIdGenerator.h"
#include "driving_adapters/matter/cluster_stubs/ClusterStubsAdapter.h"
#include "driving_adapters/matter/cover_cluster/CoverClusterAdapter.h"
#include "driving_adapters/mobilus/CompositeMobilusDeviceEventHandler.hpp"
#include "driving_adapters/mobilus/MobilusDeviceEventHandler.h"
#include "driving_adapters/mobilus/MqttMobilusDeviceEventSubscriber.h"
#include "driving_adapters/mobilus/MqttMobilusDeviceInitializer.h"
#include "driving_adapters/mobilus/MqttMobilusDeviceNameSyncer.h"
#include "driving_adapters/mobilus/MqttMobilusDeviceStateSyncer.h"
#include "driving_adapters/mobilus/cover/MobilusCoverEventHandler.h"
#include "driving_adapters/mobilus/cover/MobilusCoverInitHandler.h"
#include "driving_adapters/mobilus/cover/MobilusCoverNameHandler.h"
#include "driving_adapters/mobilus/generic/MobilusDeviceNameSyncerAdapter.h"
#include "jungi/mobilus_gtw_client/MqttDsn.h"
#include "jungi/mobilus_gtw_client/MqttMobilusGtwClient.h"
#include "jungi/mobilus_gtw_client/proto/DeviceSettingsRequest.pb.h"
#include "matter/ChipAppMain.h"
#include "matter/event_loop/DomainEventPublisherAdapter.h"
#include "matter/event_loop/MobilusGtwEventLoopAdapter.h"
#include "matter/persistence/SqlitePersistentStorageDelegate.h"

#include <platform/CHIPDeviceLayer.h>
#include <tl/expected.hpp>

#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <memory>
#include <optional>
#include <string>

using namespace mobmatter::common::persistence;
using namespace mobmatter::common::domain;
using namespace mobmatter::common::logging;
using namespace mobmatter::common::logging::handlers;
using namespace mobmatter::application;
using namespace mobmatter::application::subscribers;
using namespace mobmatter::application::driven_ports;
using namespace mobmatter::driven_adapters::persistence::sqlite;
using namespace mobmatter::driven_adapters::mobilus;
using namespace mobmatter::driven_adapters::matter::zcl;
using namespace mobmatter::driven_adapters::matter::reporting;
using namespace mobmatter::driving_adapters::mobilus;
using namespace mobmatter::driving_adapters::mobilus::generic;
using namespace mobmatter::driving_adapters::mobilus::cover;
using namespace mobmatter::driving_adapters::matter::cover_cluster;
using namespace mobmatter::driving_adapters::matter::cluster_stubs;
using namespace mobmatter::matter;
using namespace mobmatter::matter::event_loop;
using namespace mobmatter::matter::persistence;
using mobmatter::application::model::MobilusDeviceId;
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

std::unique_ptr<mobgtw::MqttMobilusGtwClient> createMobilusGtwClient(mobgtw::io::EventLoop* loop, mobgtw::logging::Logger* logger)
{
    return mobgtw::MqttMobilusGtwClient::builder()
        .dsn(mobgtw::MqttDsn::from(MOBILUS_DSN).value())
        .login({ getEnvOr("MOBILUS_USERNAME", "admin"), getEnvOr("MOBILUS_PASSWORD", "admin") })
        .useKeepAliveMessage(std::make_unique<proto::DeviceSettingsRequest>())
        .useLogger(logger)
        .attachTo(loop)
        .build();
}

tl::expected<sqlite::Connection, sqlite::SqliteError> openSqliteDb()
{
    bool initiate = !std::filesystem::exists(DATABASE_FILE);
    auto db = sqlite::Connection::open(DATABASE_FILE, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);

    if (!db) {
        return std::move(db);
    }

    if (auto r = db->exec(DATABASE_PRAGMA_SQL); !r) {
        db->close();
        std::filesystem::remove(DATABASE_FILE);

        return tl::unexpected(std::move(r.error()));
    }

    if (initiate) {
        if (auto r = db->exec(kDatabaseSchema); !r) {
            db->close();
            std::filesystem::remove(DATABASE_FILE);

            return tl::unexpected(std::move(r.error()));
        }
    }

    return std::move(db);
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
    MobilusCoverInitHandler coverInitHandler(coverRepository, endpointIdGenerator, logger);
    MqttMobilusDeviceInitializer mobilusDeviceInitializer(mobilusGtwClient, logger);
    mobilusDeviceInitializer.registerHandler(coverInitHandler);

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

void syncDeviceStates(mobgtw::MqttMobilusGtwClient& mobilusGtwClient, MobilusDeviceEventHandler& eventHandler, Logger& logger)
{
    MqttMobilusDeviceStateSyncer mobilusDeviceStateSyncer(mobilusGtwClient, eventHandler, logger);
    mobilusDeviceStateSyncer.run();
}

void showVersion()
{
    printf("mobmatter v%s\n", CHIP_DEVICE_CONFIG_DEVICE_SOFTWARE_VERSION_STRING);
}

int main(int argc, char* argv[])
{
    if (argc > 1 && !strcmp("--version", argv[1])) {
        showVersion();
        return 0;
    }

    // must outlive the main()
    static Logger logger(createLogHandler());
    static auto db = openSqliteDb();

    if (!db) {
        logger.critical("Sqlite error: %s", db.error().message().c_str());
        return 1;
    }

    auto& chipSystemLayer = static_cast<chip::System::LayerSocketsLoop&>(chip::DeviceLayer::SystemLayer());
    MobilusGtwEventLoopAdapter mobilusGtwEventLoopAdapter(chipSystemLayer);
    MqttMobilusGtwClientLoggerAdapter mobilusLoggerAdapter(logger);
    auto mobilusGtwClient = createMobilusGtwClient(&mobilusGtwEventLoopAdapter, &mobilusLoggerAdapter);

    // chip
    static SqlitePersistentStorageDelegate persistentStorageDelegate;
    persistentStorageDelegate.Init(&*db, &logger);

    // driven
    SqliteCoverRepository coverRepository(*db, logger);
    SqliteEndpointIdGenerator endpointIdGenerator(ZCL_INITIAL_DYNAMIC_ENDPOINT_ID, *db, logger);
    ZclCoverEndpointService coverEndpointService(ZCL_AGGREGATOR_ENDPOINT_ID);
    MqttMobilusCoverControlService coverControlService(*mobilusGtwClient, logger);
    MqttMobilusDeviceManagementService deviceManagementService(*mobilusGtwClient, logger);

    // app subscribers
    auto& domainEventPublisher = DomainEventPublisher::instance();
    DomainEventPublisherAdapter domainEventPublisherAdapter(chipSystemLayer);
    CoverControlSubscriber mobilusCoverControlSubscriber(coverControlService);
    CoverEndpointSubscriber chipCoverEndpointSubscriber(coverEndpointService);
    DeviceManagementSubscriber deviceManagemenetSubscriber(deviceManagementService);
    CoverReportingAdapter coverReportingAdapter;

    // driving
    CoverClusterAdapter coverClusterAdapter(coverRepository, logger);
    ClusterStubsAdapter clusterStubsAdapter;

    MobilusCoverNameHandler mobilusCoverNameHandler(coverRepository, logger);
    MqttMobilusDeviceNameSyncer mobilusDeviceNameSyncer(*mobilusGtwClient, logger);
    mobilusDeviceNameSyncer.registerHandler(mobilusCoverNameHandler);

    MobilusCoverEventHandler mobilusCoverEventHandler(coverRepository, logger);
    MobilusDeviceNameSyncerAdapter mobilusDeviceNameSyncerAdapter(mobilusDeviceNameSyncer);

    CompositeMobilusDeviceEventHandler mobilusDeviceEventHandler;
    mobilusDeviceEventHandler.registerHandler(mobilusCoverEventHandler);
    mobilusDeviceEventHandler.registerHandler(mobilusDeviceNameSyncerAdapter);

    MqttMobilusDeviceEventSubscriber mobilusDeviceEventSubscriber(*mobilusGtwClient, mobilusDeviceEventHandler);

    signal(SIGINT, handleSignal);
    signal(SIGTERM, handleSignal);

    domainEventPublisher.subscribe(mobilusCoverControlSubscriber);
    domainEventPublisher.subscribe(chipCoverEndpointSubscriber);
    domainEventPublisher.subscribe(deviceManagemenetSubscriber);
    domainEventPublisher.subscribe(coverReportingAdapter);

    sChipApp.registerComponent(mobilusGtwEventLoopAdapter);
    sChipApp.registerComponent(domainEventPublisherAdapter);
    sChipApp.registerComponent(coverClusterAdapter);
    sChipApp.registerComponent(clusterStubsAdapter);
    sChipApp.registerComponent(mobilusDeviceEventSubscriber);

    int rc = sChipApp.boot(logger, *mobilusGtwClient, persistentStorageDelegate);
    if (rc) {
        return rc;
    }

    // after ZAP boot
    if (!setupDevices(*mobilusGtwClient, coverRepository, coverEndpointService, endpointIdGenerator, logger)) {
        return 1;
    }

    syncDeviceStates(*mobilusGtwClient, mobilusDeviceEventHandler, logger);

    sChipApp.run();

    return 0;
}
