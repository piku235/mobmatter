#include "SwitchCommandHandler.h"
#include "application/model/Switch.h"

#include <app-common/zap-generated/cluster-objects.h>
#include <app-common/zap-generated/ids/Clusters.h>
#include <app-common/zap-generated/ids/Commands.h>

#include <optional>

using namespace chip;
using namespace chip::app;
using mobmatter::application::driven_ports::SwitchRepository;
using mobmatter::application::model::Switch;
using Protocols::InteractionModel::Status;

namespace mobmatter::driving_adapters::matter::switch_cluster {

SwitchCommandHandler::SwitchCommandHandler(SwitchRepository& switchRepository, logging::Logger& logger)
    : CommandHandlerInterface(Optional<EndpointId>::Missing(), Clusters::OnOff::Id)
    , mSwitchRepository(switchRepository)
    , mLogger(logger)
{
}

void SwitchCommandHandler::InvokeCommand(HandlerContext& handlerContext)
{
    using namespace Clusters::OnOff::Commands;

    auto switch_ = mSwitchRepository.find(handlerContext.mRequestPath.mEndpointId);

    handlerContext.SetCommandHandled();

    if (!switch_) {
        handlerContext.mCommandHandler.AddStatus(handlerContext.mRequestPath, Status::NotFound);
        return;
    }

    switch (handlerContext.mRequestPath.mCommandId) {
    case On::Id:
        mLogger.notice("On command received on endpoint: %u", handlerContext.mRequestPath.mEndpointId);

        if (Switch::Result::Ok == switch_->requestOn()) {
            mSwitchRepository.save(*switch_);
        }

        handlerContext.mCommandHandler.AddStatus(handlerContext.mRequestPath, Status::Success);
        break;
    case Off::Id:
        mLogger.notice("Off command received on endpoint: %u", handlerContext.mRequestPath.mEndpointId);

        if (Switch::Result::Ok == switch_->requestOff()) {
            mSwitchRepository.save(*switch_);
        }

        handlerContext.mCommandHandler.AddStatus(handlerContext.mRequestPath, Status::Success);
        break;
    case Toggle::Id:
        mLogger.notice("Toggle command received on endpoint: %u", handlerContext.mRequestPath.mEndpointId);

        if (Switch::Result::Ok == switch_->requestToggle()) {
            mSwitchRepository.save(*switch_);
        }

        handlerContext.mCommandHandler.AddStatus(handlerContext.mRequestPath, Status::Success);
        break;
    default:
        handlerContext.mCommandHandler.AddStatus(handlerContext.mRequestPath, Status::UnsupportedCommand);
        mLogger.warning("unsupported window covering command: " ChipLogFormatMEI " on endpoint: %u", ChipLogValueMEI(handlerContext.mRequestPath.mCommandId), handlerContext.mRequestPath.mEndpointId);
    }
}

}
