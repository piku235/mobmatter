#include "ClusterStubsAdapter.h"

#include <app/AttributeAccessInterfaceRegistry.h>
#include <app/CommandHandlerInterfaceRegistry.h>

using namespace chip::app;

namespace mmbridge::driving_adapters::matter::cluster_stubs {

ClusterStubsAdapter::~ClusterStubsAdapter()
{
    shutdown();
}

void ClusterStubsAdapter::boot()
{
    auto& commandHandlerRegistry = CommandHandlerInterfaceRegistry::Instance();
    auto& attributeAccessRegistry = AttributeAccessInterfaceRegistry::Instance();

    attributeAccessRegistry.Register(&mIdentifyAttributeAccess);
    commandHandlerRegistry.RegisterCommandHandler(&mIdentifyCommandHandler);
}

void ClusterStubsAdapter::shutdown()
{
    auto& commandHandlerRegistry = CommandHandlerInterfaceRegistry::Instance();
    auto& attributeAccessRegistry = AttributeAccessInterfaceRegistry::Instance();

    attributeAccessRegistry.Unregister(&mIdentifyAttributeAccess);
    commandHandlerRegistry.UnregisterCommandHandler(&mIdentifyCommandHandler);
}

}
