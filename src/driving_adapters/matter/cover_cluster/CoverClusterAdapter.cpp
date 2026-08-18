#include "CoverClusterAdapter.h"

#include "CoverBasicInfoLoader.h"

#include <app/AttributeAccessInterfaceRegistry.h>
#include <app/CommandHandlerInterfaceRegistry.h>

using namespace chip::app;
using namespace mobmatter::application::model::window_covering;
using mobmatter::application::driven_ports::CoverRepository;
using mobmatter::driving_adapters::matter::bridged_device_cluster::BridgedDeviceBasicInfoAttributeAccessRegistry;

namespace mobmatter::driving_adapters::matter::cover_cluster {

CoverClusterAdapter::CoverClusterAdapter(CoverRepository& coverRepository, BridgedDeviceBasicInfoAttributeAccessRegistry& basicInfoAttributeAccessRegistry, logging::Logger& logger)
    : mCoverRepository(coverRepository)
    , mBasicInfoLoader(coverRepository)
    , mBasicInfoAttributeAccessRegistry(basicInfoAttributeAccessRegistry)
    , mCoverAttributeAccess(coverRepository)
    , mCoverCommandHandler(coverRepository, logger)
{
}

void CoverClusterAdapter::boot()
{
    auto& commandHandlerRegistry = CommandHandlerInterfaceRegistry::Instance();
    auto& attributeAccessRegistry = AttributeAccessInterfaceRegistry::Instance();

    attributeAccessRegistry.Register(&mCoverAttributeAccess);
    commandHandlerRegistry.RegisterCommandHandler(&mCoverCommandHandler);

    for (auto& cover : mCoverRepository.all()) {
        mBasicInfoAttributeAccessRegistry.registerAttributeAccess(cover.endpointId(), mBasicInfoLoader);
    }
}

void CoverClusterAdapter::shutdown()
{
    auto& commandHandlerRegistry = CommandHandlerInterfaceRegistry::Instance();
    auto& attributeAccessRegistry = AttributeAccessInterfaceRegistry::Instance();

    attributeAccessRegistry.Unregister(&mCoverAttributeAccess);
    commandHandlerRegistry.UnregisterCommandHandler(&mCoverCommandHandler);

    for (auto& cover : mCoverRepository.all()) {
        mBasicInfoAttributeAccessRegistry.unregisterAttributeAccess(cover.endpointId());
    }
}

void CoverClusterAdapter::handle(const CoverAdded& event)
{
    mBasicInfoAttributeAccessRegistry.registerAttributeAccess(event.endpointId, mBasicInfoLoader);
}

void CoverClusterAdapter::handle(const CoverRemoved& event)
{
    mBasicInfoAttributeAccessRegistry.unregisterAttributeAccess(event.endpointId);
}

}
