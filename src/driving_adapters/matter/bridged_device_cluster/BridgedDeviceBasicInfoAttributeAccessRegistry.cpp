#include "BridgedDeviceBasicInfoAttributeAccessRegistry.h"

#include <app/AttributeAccessInterfaceRegistry.h>

using namespace chip::app;

namespace mobmatter::driving_adapters::matter::bridged_device_cluster {

void BridgedDeviceBasicInfoAttributeAccessRegistry::registerAttributeAccess(chip::EndpointId endpointId, BridgedDeviceBasicInfoLoader& loader)
{
    auto& registry = AttributeAccessInterfaceRegistry::Instance();

    for (auto& attributeAccess : mAttributeAccessList) {
        if (!attributeAccess) {
            registry.Register(&attributeAccess.emplace(endpointId, loader));
            break;
        }
    }
}

void BridgedDeviceBasicInfoAttributeAccessRegistry::unregisterAttributeAccess(chip::EndpointId endpointId)
{
    auto& registry = AttributeAccessInterfaceRegistry::Instance();

    for (auto& attributeAccess : mAttributeAccessList) {
        if (attributeAccess && attributeAccess->MatchesEndpoint(endpointId)) {
            registry.Unregister(&*attributeAccess);
            attributeAccess.reset();
            break;
        }
    }
}

}
