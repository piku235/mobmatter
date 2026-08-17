#include "SwitchClusterAdapter.h"

#include <app/AttributeAccessInterfaceRegistry.h>
#include <app/CommandHandlerInterfaceRegistry.h>

using namespace chip::app;
using namespace mobmatter::application::model;
using mobmatter::application::driven_ports::SwitchRepository;

namespace mobmatter::driving_adapters::matter::switch_cluster {

SwitchClusterAdapter::SwitchClusterAdapter(SwitchRepository& switchRepository, logging::Logger& logger)
    : mSwitchRepository(switchRepository)
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
        registerBridgedDeviceBasicInfoAttributeAccessFor(switch_.endpointId());
    }
}

void SwitchClusterAdapter::shutdown()
{
    auto& commandHandlerRegistry = CommandHandlerInterfaceRegistry::Instance();
    auto& attributeAccessRegistry = AttributeAccessInterfaceRegistry::Instance();

    attributeAccessRegistry.Unregister(&mSwitchAttributeAccess);
    commandHandlerRegistry.UnregisterCommandHandler(&mSwitchCommandHandler);

    for (auto& switch_ : mSwitchRepository.all()) {
        unregisterBridgedDeviceBasicInfoAttributeAccessFor(switch_.endpointId());
    }
}

void SwitchClusterAdapter::handle(const SwitchAdded& event)
{
    registerBridgedDeviceBasicInfoAttributeAccessFor(event.endpointId);
}

void SwitchClusterAdapter::handle(const SwitchRemoved& event)
{
    unregisterBridgedDeviceBasicInfoAttributeAccessFor(event.endpointId);
}

void SwitchClusterAdapter::registerBridgedDeviceBasicInfoAttributeAccessFor(chip::EndpointId endpointId)
{
    auto& registry = AttributeAccessInterfaceRegistry::Instance();

    for (auto& attributeAccess : mBridgeDeviceBasicInfoAttributeAccessList) {
        if (!attributeAccess) {
            registry.Register(&attributeAccess.emplace(endpointId, mSwitchRepository));
            break;
        }
    }
}

void SwitchClusterAdapter::unregisterBridgedDeviceBasicInfoAttributeAccessFor(chip::EndpointId endpointId)
{
    auto& registry = AttributeAccessInterfaceRegistry::Instance();

    for (auto& attributeAccess : mBridgeDeviceBasicInfoAttributeAccessList) {
        if (attributeAccess && attributeAccess->MatchesEndpoint(endpointId)) {
            registry.Unregister(&*attributeAccess);
            attributeAccess.reset();
            break;
        }
    }
}

}
