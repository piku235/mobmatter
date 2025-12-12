#pragma once

#include "IdentifyAttributeAccessStub.h"
#include "IdentifyCommandHandlerStub.h"
#include "matter/AppComponent.h"

namespace mobmatter::driving_adapters::matter::cluster_stubs {

class ClusterStubsAdapter final : public mobmatter::matter::AppComponent {
public:
    void boot() override;
    void shutdown() override;

private:
    IdentifyAttributeAccessStub mIdentifyAttributeAccess;
    IdentifyCommandHandlerStub mIdentifyCommandHandler;
};

}
