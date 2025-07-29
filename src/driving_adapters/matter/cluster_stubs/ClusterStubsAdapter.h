#pragma once

#include "IdentifyAttributeAccessStub.h"
#include "IdentifyCommandHandlerStub.h"
#include "matter/AppComponent.h"

namespace mmbridge::driving_adapters::matter::cluster_stubs {

class ClusterStubsAdapter final : public mmbridge::matter::AppComponent {
public:
    ~ClusterStubsAdapter();
    void boot() override;
    void shutdown() override;

private:
    IdentifyAttributeAccessStub mIdentifyAttributeAccess;
    IdentifyCommandHandlerStub mIdentifyCommandHandler;
};

}
