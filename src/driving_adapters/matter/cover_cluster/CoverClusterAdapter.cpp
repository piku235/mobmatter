#include "CoverClusterAdapter.h"

#include <app/AttributeAccessInterfaceRegistry.h>
#include <app/CommandHandlerInterfaceRegistry.h>

using namespace chip::app;
using mobmatter::application::driven_ports::CoverRepository;

namespace mobmatter::driving_adapters::matter::cover_cluster {

CoverClusterAdapter::CoverClusterAdapter(CoverRepository& coverRepository, logging::Logger& logger)
    : mCoverAttributeAccess(coverRepository)
    , mCoverCommandHandler(coverRepository, logger)
    , mBridgedDeviceBasicInfoAttributeAccess(coverRepository)
{
}

void CoverClusterAdapter::boot()
{
    auto& commandHandlerRegistry = CommandHandlerInterfaceRegistry::Instance();
    auto& attributeAccessRegistry = AttributeAccessInterfaceRegistry::Instance();

    attributeAccessRegistry.Register(&mCoverAttributeAccess);
    attributeAccessRegistry.Register(&mBridgedDeviceBasicInfoAttributeAccess);
    commandHandlerRegistry.RegisterCommandHandler(&mCoverCommandHandler);
}

void CoverClusterAdapter::shutdown()
{
    auto& commandHandlerRegistry = CommandHandlerInterfaceRegistry::Instance();
    auto& attributeAccessRegistry = AttributeAccessInterfaceRegistry::Instance();

    attributeAccessRegistry.Unregister(&mCoverAttributeAccess);
    attributeAccessRegistry.Unregister(&mBridgedDeviceBasicInfoAttributeAccess);
    commandHandlerRegistry.UnregisterCommandHandler(&mCoverCommandHandler);
}

}
