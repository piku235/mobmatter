#include "SwitchClusterAdapter.h"

#include <app/AttributeAccessInterfaceRegistry.h>
#include <app/CommandHandlerInterfaceRegistry.h>

using namespace chip::app;
using mobmatter::application::driven_ports::SwitchRepository;

namespace mobmatter::driving_adapters::matter::switch_cluster {

SwitchClusterAdapter::SwitchClusterAdapter(SwitchRepository& switchRepository, logging::Logger& logger)
    : mSwitchAttributeAccess(switchRepository)
    , mSwitchCommandHandler(switchRepository, logger)
{
}

void SwitchClusterAdapter::boot()
{
    auto& commandHandlerRegistry = CommandHandlerInterfaceRegistry::Instance();
    auto& attributeAccessRegistry = AttributeAccessInterfaceRegistry::Instance();

    attributeAccessRegistry.Register(&mSwitchAttributeAccess);
    commandHandlerRegistry.RegisterCommandHandler(&mSwitchCommandHandler);
}

void SwitchClusterAdapter::shutdown()
{
    auto& commandHandlerRegistry = CommandHandlerInterfaceRegistry::Instance();
    auto& attributeAccessRegistry = AttributeAccessInterfaceRegistry::Instance();

    attributeAccessRegistry.Unregister(&mSwitchAttributeAccess);
    commandHandlerRegistry.UnregisterCommandHandler(&mSwitchCommandHandler);
}

}
