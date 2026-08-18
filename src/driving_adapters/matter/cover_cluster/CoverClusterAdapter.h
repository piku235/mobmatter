#pragma once

#include "CoverAttributeAccess.h"
#include "CoverBasicInfoLoader.h"
#include "CoverCommandHandler.h"
#include "application/driven_ports/CoverRepository.h"
#include "application/model/window_covering/CoverEvents.h"
#include "common/domain/MultiDomainEventSubscriber.h"
#include "common/logging/Logger.h"
#include "driving_adapters/matter/bridged_device_cluster/BridgedDeviceBasicInfoAttributeAccessRegistry.h"
#include "matter/AppComponent.h"

namespace mobmatter::driving_adapters::matter::cover_cluster {

namespace driven_ports = application::driven_ports;
namespace logging = common::logging;

class CoverClusterAdapter final : public mobmatter::matter::AppComponent,
                                  public common::domain::MultiDomainEventSubscriber<model::window_covering::CoverAdded, model::window_covering::CoverRemoved> {
public:
    CoverClusterAdapter(driven_ports::CoverRepository& coverRepository, bridged_device_cluster::BridgedDeviceBasicInfoAttributeAccessRegistry& basicInfoAttributeAccessRegistry, logging::Logger& logger);

    void boot() override;
    void shutdown() override;

    void handle(const model::window_covering::CoverAdded& event) override;
    void handle(const model::window_covering::CoverRemoved& event) override;

private:
    driven_ports::CoverRepository& mCoverRepository;
    CoverBasicInfoLoader mBasicInfoLoader;
    bridged_device_cluster::BridgedDeviceBasicInfoAttributeAccessRegistry& mBasicInfoAttributeAccessRegistry;
    CoverAttributeAccess mCoverAttributeAccess;
    CoverCommandHandler mCoverCommandHandler;
};

}
