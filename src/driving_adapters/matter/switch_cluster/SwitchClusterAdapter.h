#pragma once

#include "SwitchAttributeAccess.h"
#include "SwitchBasicInfoLoader.h"
#include "SwitchCommandHandler.h"
#include "application/driven_ports/SwitchRepository.h"
#include "application/model/SwitchEvents.h"
#include "common/domain/MultiDomainEventSubscriber.h"
#include "common/logging/Logger.h"
#include "driving_adapters/matter/bridged_device_cluster/BridgedDeviceBasicInfoAttributeAccessRegistry.h"
#include "matter/AppComponent.h"

namespace mobmatter::driving_adapters::matter::switch_cluster {

namespace driven_ports = application::driven_ports;
namespace logging = common::logging;

class SwitchClusterAdapter final : public mobmatter::matter::AppComponent,
                                   public common::domain::MultiDomainEventSubscriber<model::SwitchAdded, model::SwitchRemoved> {
public:
    SwitchClusterAdapter(driven_ports::SwitchRepository& switchRepository, bridged_device_cluster::BridgedDeviceBasicInfoAttributeAccessRegistry& basicInfoAttributeAccessRegistry, logging::Logger& logger);

    void boot() override;
    void shutdown() override;

    void handle(const model::SwitchAdded& event) override;
    void handle(const model::SwitchRemoved& event) override;

private:
    driven_ports::SwitchRepository& mSwitchRepository;
    SwitchBasicInfoLoader mBasicInfoLoader;
    bridged_device_cluster::BridgedDeviceBasicInfoAttributeAccessRegistry& mBasicInfoAttributeAccessRegistry;
    SwitchAttributeAccess mSwitchAttributeAccess;
    SwitchCommandHandler mSwitchCommandHandler;
};

}
