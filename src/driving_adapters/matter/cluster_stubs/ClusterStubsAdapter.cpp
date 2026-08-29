#include "ClusterStubsAdapter.h"

#include <app/AttributeAccessInterfaceRegistry.h>
#include <app/CommandHandlerInterfaceRegistry.h>

using namespace chip::app;

namespace mobmatter::driving_adapters::matter::cluster_stubs {

void ClusterStubsAdapter::boot()
{
    auto& commandHandlerRegistry = CommandHandlerInterfaceRegistry::Instance();
    auto& attributeAccessRegistry = AttributeAccessInterfaceRegistry::Instance();

    attributeAccessRegistry.Register(&mIdentifyAttributeAccess);
    (void)commandHandlerRegistry.RegisterCommandHandler(&mIdentifyCommandHandler);
}

void ClusterStubsAdapter::shutdown()
{
    auto& commandHandlerRegistry = CommandHandlerInterfaceRegistry::Instance();
    auto& attributeAccessRegistry = AttributeAccessInterfaceRegistry::Instance();

    attributeAccessRegistry.Unregister(&mIdentifyAttributeAccess);
    (void)commandHandlerRegistry.UnregisterCommandHandler(&mIdentifyCommandHandler);
}

}
