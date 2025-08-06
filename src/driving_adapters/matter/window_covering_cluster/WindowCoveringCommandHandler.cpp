#include "WindowCoveringCommandHandler.h"
#include "application/model/window_covering/Position.h"

#include <app-common/zap-generated/cluster-objects.h>
#include <app-common/zap-generated/ids/Clusters.h>
#include <app-common/zap-generated/ids/Commands.h>

#include <optional>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::WindowCovering::Commands;
using mmbridge::application::driven_ports::CoverRepository;
using AppPercent = mmbridge::application::model::Percent;
using mmbridge::application::model::window_covering::Position;
using Protocols::InteractionModel::Status;

namespace mmbridge::driving_adapters::matter::window_covering_cluster {

WindowCoveringCommandHandler::WindowCoveringCommandHandler(CoverRepository& coverRepository, logging::Logger& logger)
    : CommandHandlerInterface(Optional<EndpointId>::Missing(), WindowCovering::Id)
    , mCoverRepository(coverRepository)
    , mLogger(logger)
{
}

void WindowCoveringCommandHandler::InvokeCommand(HandlerContext& handlerContext)
{
    auto cover = mCoverRepository.find(handlerContext.mRequestPath.mEndpointId);

    handlerContext.SetCommandHandled();

    if (!cover) {
        handlerContext.mCommandHandler.AddStatus(handlerContext.mRequestPath, Status::NotFound);
        return;
    }

    switch (handlerContext.mRequestPath.mCommandId) {
    case UpOrOpen::Id:
        mLogger.notice("UpOrOpen command received on endpoint: %u", handlerContext.mRequestPath.mEndpointId);

        cover->requestOpen();
        mCoverRepository.save(*cover);

        handlerContext.mCommandHandler.AddStatus(handlerContext.mRequestPath, Status::Success);

        break;
    case DownOrClose::Id:
        mLogger.notice("DownOrClose command received on endpoint: %u", handlerContext.mRequestPath.mEndpointId);

        cover->requestClose();
        mCoverRepository.save(*cover);

        handlerContext.mCommandHandler.AddStatus(handlerContext.mRequestPath, Status::Success);

        break;
    case StopMotion::Id:
        mLogger.notice("StopMotion command received on endpoint: %u", handlerContext.mRequestPath.mEndpointId);

        cover->requestStopMotion();
        mCoverRepository.save(*cover);

        handlerContext.mCommandHandler.AddStatus(handlerContext.mRequestPath, Status::Success);

        break;
    case GoToLiftPercentage::Id: {
        GoToLiftPercentage::DecodableType data;

        mLogger.notice("GoToLiftPercentage command received on endpoint: %u", handlerContext.mRequestPath.mEndpointId);

        if (DataModel::Decode(handlerContext.mPayload, data) != CHIP_NO_ERROR) {
            handlerContext.mCommandHandler.AddStatus(handlerContext.mRequestPath, Status::InvalidCommand);
            break;
        }

        auto percent = AppPercent::from100ths(data.liftPercent100thsValue);

        if (!percent) {
            handlerContext.mCommandHandler.AddStatus(handlerContext.mRequestPath, Status::ConstraintError);
            break;
        }

        if (auto e = cover->requestLiftTo(Position::closed(*percent)); !e) {
            mLogger.error("GoToLiftPercentage command for endpoint: %u failed: %s", handlerContext.mRequestPath.mEndpointId, e.error().message().c_str());
            handlerContext.mCommandHandler.AddStatus(handlerContext.mRequestPath, Status::Failure);
            break;
        }

        mCoverRepository.save(*cover);
        handlerContext.mCommandHandler.AddStatus(handlerContext.mRequestPath, Status::Success);

        break;
    }
    default:
        handlerContext.mCommandHandler.AddStatus(handlerContext.mRequestPath, Status::UnsupportedCommand);
        mLogger.warning("unsupported window covering command: " ChipLogFormatMEI " on endpoint: %u", ChipLogValueMEI(handlerContext.mRequestPath.mCommandId), handlerContext.mRequestPath.mEndpointId);
    }
}

}
