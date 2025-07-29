#include "WindowCoveringClusterAdapter.h"

#include <app/AttributeAccessInterfaceRegistry.h>
#include <app/CommandHandlerInterfaceRegistry.h>

using namespace chip::app;
using mmbridge::application::driven_ports::CoverRepository;

namespace mmbridge::driving_adapters::matter::window_covering_cluster {

WindowCoveringClusterAdapter::WindowCoveringClusterAdapter(CoverRepository& coverRepository, logging::Logger& logger)
    : mWindowCoveringAttributeAccess(coverRepository)
    , mWindowCoveringCommandHandler(coverRepository, logger)
    , mBridgedDeviceBasicInfoAttributeAccess(coverRepository)
{
}

WindowCoveringClusterAdapter::~WindowCoveringClusterAdapter()
{
    shutdown();
}

void WindowCoveringClusterAdapter::boot()
{
    auto& commandHandlerRegistry = CommandHandlerInterfaceRegistry::Instance();
    auto& attributeAccessRegistry = AttributeAccessInterfaceRegistry::Instance();

    attributeAccessRegistry.Register(&mWindowCoveringAttributeAccess);
    attributeAccessRegistry.Register(&mBridgedDeviceBasicInfoAttributeAccess);
    commandHandlerRegistry.RegisterCommandHandler(&mWindowCoveringCommandHandler);
}

void WindowCoveringClusterAdapter::shutdown()
{
    auto& commandHandlerRegistry = CommandHandlerInterfaceRegistry::Instance();
    auto& attributeAccessRegistry = AttributeAccessInterfaceRegistry::Instance();

    attributeAccessRegistry.Unregister(&mWindowCoveringAttributeAccess);
    attributeAccessRegistry.Unregister(&mBridgedDeviceBasicInfoAttributeAccess);
    commandHandlerRegistry.UnregisterCommandHandler(&mWindowCoveringCommandHandler);
}

}
