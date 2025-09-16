#pragma once

#include <app-common/zap-generated/ids/Clusters.h>
#include <app-common/zap-generated/ids/Commands.h>
#include <app/CommandHandlerInterface.h>

namespace mobmatter::driving_adapters::matter::cluster_stubs {

class IdentifyCommandHandlerStub final : public chip::app::CommandHandlerInterface {
public:
    IdentifyCommandHandlerStub()
        : CommandHandlerInterface(chip::Optional<chip::EndpointId>::Missing(), chip::app::Clusters::Identify::Id)
    {
    }

    void InvokeCommand(HandlerContext& handlerContext) override
    {
        using namespace chip::app::Clusters::Identify::Commands;
        using chip::Protocols::InteractionModel::Status;

        handlerContext.SetCommandHandled();
        handlerContext.mCommandHandler.AddStatus(handlerContext.mRequestPath, Identify::Id == handlerContext.mRequestPath.mCommandId ? Status::Success : Status::UnsupportedCommand);
    }
};

}
