#include "CoverClusterAdapter.h"

#include <app/AttributeAccessInterfaceRegistry.h>
#include <app/CommandHandlerInterfaceRegistry.h>

using namespace chip::app;
using namespace mobmatter::application::model::window_covering;
using mobmatter::application::driven_ports::CoverRepository;

namespace mobmatter::driving_adapters::matter::cover_cluster {

CoverClusterAdapter::CoverClusterAdapter(CoverRepository& coverRepository, logging::Logger& logger)
    : mCoverRepository(coverRepository)
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
        registerBridgedDeviceBasicInfoAttributeAccessFor(cover.endpointId());
    }
}

void CoverClusterAdapter::shutdown()
{
    auto& commandHandlerRegistry = CommandHandlerInterfaceRegistry::Instance();
    auto& attributeAccessRegistry = AttributeAccessInterfaceRegistry::Instance();

    attributeAccessRegistry.Unregister(&mCoverAttributeAccess);
    commandHandlerRegistry.UnregisterCommandHandler(&mCoverCommandHandler);

    for (auto& cover : mCoverRepository.all()) {
        unregisterBridgedDeviceBasicInfoAttributeAccessFor(cover.endpointId());
    }
}

void CoverClusterAdapter::handle(const CoverAdded& event)
{
    registerBridgedDeviceBasicInfoAttributeAccessFor(event.endpointId);
}

void CoverClusterAdapter::handle(const CoverRemoved& event)
{
    unregisterBridgedDeviceBasicInfoAttributeAccessFor(event.endpointId);
}

void CoverClusterAdapter::registerBridgedDeviceBasicInfoAttributeAccessFor(chip::EndpointId endpointId)
{
    auto& registry = AttributeAccessInterfaceRegistry::Instance();

    for (auto& attributeAccess : mBridgeDeviceBasicInfoAttributeAccessList) {
        if (!attributeAccess) {
            registry.Register(&attributeAccess.emplace(endpointId, mCoverRepository));
            break;
        }
    }
}

void CoverClusterAdapter::unregisterBridgedDeviceBasicInfoAttributeAccessFor(chip::EndpointId endpointId)
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
