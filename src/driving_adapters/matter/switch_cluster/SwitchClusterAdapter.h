#pragma once

#include "BridgedDeviceBasicInfoAttributeAccess.h"
#include "SwitchAttributeAccess.h"
#include "SwitchCommandHandler.h"
#include "application/driven_ports/SwitchRepository.h"
#include "application/model/SwitchEvents.h"
#include "common/domain/MultiDomainEventSubscriber.h"
#include "common/logging/Logger.h"
#include "matter/AppComponent.h"

#include <optional>

namespace mobmatter::driving_adapters::matter::switch_cluster {

namespace driven_ports = application::driven_ports;
namespace logging = common::logging;

class SwitchClusterAdapter final : public mobmatter::matter::AppComponent,
                                   public common::domain::MultiDomainEventSubscriber<model::SwitchAdded, model::SwitchRemoved> {
public:
    SwitchClusterAdapter(driven_ports::SwitchRepository& switchRepository, logging::Logger& logger);

    void boot() override;
    void shutdown() override;

    void handle(const model::SwitchAdded& event) override;
    void handle(const model::SwitchRemoved& event) override;

private:
    driven_ports::SwitchRepository& mSwitchRepository;
    SwitchAttributeAccess mSwitchAttributeAccess;
    SwitchCommandHandler mSwitchCommandHandler;
    std::optional<BridgedDeviceBasicInfoAttributeAccess> mBridgeDeviceBasicInfoAttributeAccessList[CHIP_DEVICE_CONFIG_DYNAMIC_ENDPOINT_COUNT];

    void registerBridgedDeviceBasicInfoAttributeAccessFor(chip::EndpointId endpointId);
    void unregisterBridgedDeviceBasicInfoAttributeAccessFor(chip::EndpointId endpointId);
};

}
