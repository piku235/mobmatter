#include "SwitchClusterAdapter.h"

#include <app/AttributeAccessInterfaceRegistry.h>
#include <app/CommandHandlerInterfaceRegistry.h>

using namespace chip::app;
using namespace mobmatter::application::model;
using mobmatter::application::driven_ports::SwitchRepository;
using mobmatter::driving_adapters::matter::bridged_device_cluster::BridgedDeviceBasicInfoAttributeAccessRegistry;

namespace mobmatter::driving_adapters::matter::switch_cluster {

SwitchClusterAdapter::SwitchClusterAdapter(SwitchRepository& switchRepository, BridgedDeviceBasicInfoAttributeAccessRegistry& basicInfoAttributeAccessRegistry, logging::Logger& logger)
    : mSwitchRepository(switchRepository)
    , mBasicInfoLoader(switchRepository)
    , mBasicInfoAttributeAccessRegistry(basicInfoAttributeAccessRegistry)
    , mSwitchAttributeAccess(switchRepository)
    , mSwitchCommandHandler(switchRepository, logger)
{
}

void SwitchClusterAdapter::boot()
{
    auto& commandHandlerRegistry = CommandHandlerInterfaceRegistry::Instance();
    auto& attributeAccessRegistry = AttributeAccessInterfaceRegistry::Instance();

    attributeAccessRegistry.Register(&mSwitchAttributeAccess);
    commandHandlerRegistry.RegisterCommandHandler(&mSwitchCommandHandler);

    for (auto& switch_ : mSwitchRepository.all()) {
        mBasicInfoAttributeAccessRegistry.registerAttributeAccess(switch_.endpointId(), mBasicInfoLoader);
    }
}

void SwitchClusterAdapter::shutdown()
{
    auto& commandHandlerRegistry = CommandHandlerInterfaceRegistry::Instance();
    auto& attributeAccessRegistry = AttributeAccessInterfaceRegistry::Instance();

    attributeAccessRegistry.Unregister(&mSwitchAttributeAccess);
    commandHandlerRegistry.UnregisterCommandHandler(&mSwitchCommandHandler);

    for (auto& switch_ : mSwitchRepository.all()) {
        mBasicInfoAttributeAccessRegistry.unregisterAttributeAccess(switch_.endpointId());
    }
}

void SwitchClusterAdapter::handle(const SwitchAdded& event)
{
    mBasicInfoAttributeAccessRegistry.registerAttributeAccess(event.endpointId, mBasicInfoLoader);
}

void SwitchClusterAdapter::handle(const SwitchRemoved& event)
{
    mBasicInfoAttributeAccessRegistry.unregisterAttributeAccess(event.endpointId);
}

}
