#include "ChipAppMain.h"
#include "AppConfig.h"
#include "CommissionableDataProviderImpl.h"
#include "DacProviderImpl.h"
#include "DeviceInfoProviderImpl.h"
#include "DeviceInstanceInfoProviderImpl.h"
#include "logging/LoggerAdapter.h"

#include <platform/CHIPDeviceLayer.h>
#include <platform/ConfigurationManager.h>
#include <platform/DeviceInstanceInfoProvider.h>
#include <platform/KeyValueStoreManager.h>
#include <platform/Linux/NetworkCommissioningDriver.h>
#include <platform/PlatformManager.h>

#include <app/SimpleSubscriptionResumptionStorage.h>
#include <app/TimerDelegates.h>
#include <app/clusters/network-commissioning/network-commissioning.h>
#include <app/reporting/ReportSchedulerImpl.h>
#include <app/server/AclStorage.h>
#include <app/server/DefaultAclStorage.h>
#include <app/server/Server.h>
#include <app/util/af-types.h>
#include <app/util/persistence/DefaultAttributePersistenceProvider.h>
#include <credentials/DeviceAttestationCredsProvider.h>
#include <credentials/GroupDataProviderImpl.h>
#include <credentials/PersistentStorageOpCertStore.h>
#include <crypto/DefaultSessionKeystore.h>
#include <crypto/PersistentStorageOperationalKeystore.h>
#include <data-model-providers/codegen/Instance.h>
#include <lib/core/CHIPError.h>
#include <protocols/secure_channel/SimpleSessionResumptionStorage.h>
#include <system/SystemLayer.h>

#define RETURN_CHIP_ERROR_ON_FAILURE(err)     \
    do {                                      \
        if (CHIP_NO_ERROR != err) {           \
            return err.AsInteger();           \
        }                                     \
    } while (0)

using namespace mmbridge::common::logging;
using namespace mmbridge::matter::logging;
using jungi::mobilus_gtw_client::MqttMobilusGtwClient;

static mmbridge::matter::CommissionableDataProviderImpl sCommissionableDataProvider;
static mmbridge::matter::DeviceInfoProviderImpl sDeviceInfoProvider;
static mmbridge::matter::DeviceInstanceInfoProviderImpl sDeviceInstanceInfoProvider;
static mmbridge::matter::DacProviderImpl sDacProvider;
static chip::ServerInitParams sServerInitParams;
static chip::PersistentStorageOperationalKeystore sOperationalKeystore;
static chip::Credentials::PersistentStorageOpCertStore sOperationalCertStore;
static chip::app::DefaultTimerDelegate sTimerDelegate;
static chip::app::reporting::ReportSchedulerImpl sReportScheduler(&sTimerDelegate);
static chip::Crypto::DefaultSessionKeystore sSessionKeystore;
static chip::Credentials::GroupDataProviderImpl sGroupDataProvider;
static chip::SimpleSessionResumptionStorage sSessionResumptionStorage;
static chip::app::DefaultAclStorage sAclStorage;
static chip::app::SimpleSubscriptionResumptionStorage sSubscriptionResumptionStorage;
static chip::DeviceLayer::NetworkCommissioning::LinuxEthernetDriver sEthernetDriver;
static chip::app::DefaultAttributePersistenceProvider sAttributePersistenceProvider;
static chip::app::Clusters::NetworkCommissioning::Instance sEthernetNetworkCommissioningInstance(chip::kRootEndpointId, &sEthernetDriver);

namespace mmbridge::matter {

int ChipAppMain::boot(Logger& logger, MqttMobilusGtwClient& mobilusGtwClient, chip::PersistentStorageDelegate& persistentStorageDelegate)
{
    LoggerAdapter::setLogger(&logger);
    CHIP_ERROR err;

    err = chip::Platform::MemoryInit();
    RETURN_CHIP_ERROR_ON_FAILURE(err);

    err = chip::DeviceLayer::PlatformMgr().InitChipStack();
    RETURN_CHIP_ERROR_ON_FAILURE(err);

    err = sCommissionableDataProvider.Init();
    RETURN_CHIP_ERROR_ON_FAILURE(err);

    err = sAttributePersistenceProvider.Init(&persistentStorageDelegate);
    RETURN_CHIP_ERROR_ON_FAILURE(err);

    err = sOperationalKeystore.Init(&persistentStorageDelegate);
    RETURN_CHIP_ERROR_ON_FAILURE(err);

    err = sOperationalCertStore.Init(&persistentStorageDelegate);
    RETURN_CHIP_ERROR_ON_FAILURE(err);

    err = sSessionResumptionStorage.Init(&persistentStorageDelegate);
    RETURN_CHIP_ERROR_ON_FAILURE(err);

    err = sSubscriptionResumptionStorage.Init(&persistentStorageDelegate);
    RETURN_CHIP_ERROR_ON_FAILURE(err);

    sGroupDataProvider.SetStorageDelegate(&persistentStorageDelegate);
    sGroupDataProvider.SetSessionKeystore(&sSessionKeystore);
    
    err = sGroupDataProvider.Init();
    RETURN_CHIP_ERROR_ON_FAILURE(err);

    chip::DeviceLayer::SetDeviceInfoProvider(&sDeviceInfoProvider);
    chip::DeviceLayer::SetDeviceInstanceInfoProvider(&sDeviceInstanceInfoProvider);
    chip::DeviceLayer::SetCommissionableDataProvider(&sCommissionableDataProvider);
    chip::Credentials::SetDeviceAttestationCredentialsProvider(&sDacProvider);
    chip::app::SetAttributePersistenceProvider(&sAttributePersistenceProvider);

    if (auto e = mobilusGtwClient.connect(); !e) {
        return static_cast<int>(e.error().code());
    }

    sDeviceInstanceInfoProvider.SetSerialNumber(mobilusGtwClient.sessionInfo()->serialNumber);

    sServerInitParams.persistentStorageDelegate = &persistentStorageDelegate;
    sServerInitParams.operationalKeystore = &sOperationalKeystore;
    sServerInitParams.opCertStore = &sOperationalCertStore;
    sServerInitParams.reportScheduler = &sReportScheduler;
    sServerInitParams.sessionKeystore = &sSessionKeystore;
    sServerInitParams.groupDataProvider = &sGroupDataProvider;
    sServerInitParams.aclStorage = &sAclStorage;
    sServerInitParams.accessDelegate = chip::Access::Examples::GetAccessControlDelegate();
    sServerInitParams.subscriptionResumptionStorage = &sSubscriptionResumptionStorage;
    sServerInitParams.dataModelProvider = chip::app::CodegenDataModelProviderInstance(&persistentStorageDelegate);

    // runs server and loads ZAP
    err = chip::Server::GetInstance().Init(sServerInitParams);
    RETURN_CHIP_ERROR_ON_FAILURE(err);

    chip::DeviceLayer::ConfigurationMgr().LogDeviceConfig();

    err = sEthernetNetworkCommissioningInstance.Init();
    RETURN_CHIP_ERROR_ON_FAILURE(err);

    // boot components
    for (auto component : mComponents) {
        component->boot();
    }

    return 0;
}

void ChipAppMain::run()
{
    // run
    for (auto component : mComponents) {
        component->run();
    }

    chip::DeviceLayer::PlatformMgr().RunEventLoop();

    // shutdown
    for (auto component : mComponents) {
        component->shutdown();
    }

    chip::Server::GetInstance().Shutdown();
    chip::DeviceLayer::PlatformMgr().Shutdown();
}

void ChipAppMain::shutdown()
{
    chip::Server::GetInstance().GenerateShutDownEvent();
    chip::DeviceLayer::PlatformMgr().ScheduleWork([](intptr_t) {
        chip::DeviceLayer::PlatformMgr().StopEventLoopTask();
    });
}

}
