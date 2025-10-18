#include "CoverCommandHandler.h"
#include "application/model/window_covering/Cover.h"
#include "application/model/window_covering/Position.h"

#include <app-common/zap-generated/cluster-objects.h>
#include <app-common/zap-generated/ids/Clusters.h>
#include <app-common/zap-generated/ids/Commands.h>

#include <optional>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::WindowCovering::Commands;
using namespace mobmatter::application::model::window_covering;
using mobmatter::application::driven_ports::CoverRepository;
using AppPercent = mobmatter::application::model::Percent;
using Protocols::InteractionModel::Status;

namespace mobmatter::driving_adapters::matter::cover_cluster {

CoverCommandHandler::CoverCommandHandler(CoverRepository& coverRepository, logging::Logger& logger)
    : CommandHandlerInterface(Optional<EndpointId>::Missing(), WindowCovering::Id)
    , mCoverRepository(coverRepository)
    , mLogger(logger)
{
}

void CoverCommandHandler::InvokeCommand(HandlerContext& handlerContext)
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

        if (Cover::Result::Ok == cover->requestOpen()) {
            mCoverRepository.save(*cover);
        }

        handlerContext.mCommandHandler.AddStatus(handlerContext.mRequestPath, Status::Success);

        break;
    case DownOrClose::Id:
        mLogger.notice("DownOrClose command received on endpoint: %u", handlerContext.mRequestPath.mEndpointId);

        if (Cover::Result::Ok == cover->requestClose()) {
            mCoverRepository.save(*cover);
        }

        handlerContext.mCommandHandler.AddStatus(handlerContext.mRequestPath, Status::Success);

        break;
    case StopMotion::Id:
        mLogger.notice("StopMotion command received on endpoint: %u", handlerContext.mRequestPath.mEndpointId);

        if (Cover::Result::Ok == cover->requestStopMotion()) {
            mCoverRepository.save(*cover);
        }

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

        auto r = cover->requestLiftTo(Position::closed(*percent));

        if (Cover::Result::Ok != r && Cover::Result::NoChange != r) {
            mLogger.error("GoToLiftPercentage command for endpoint: %u failed: %u", handlerContext.mRequestPath.mEndpointId, r);
            handlerContext.mCommandHandler.AddStatus(handlerContext.mRequestPath, Status::Failure);
            break;
        }

        if (Cover::Result::Ok == r) {
            mCoverRepository.save(*cover);
        }
            
        handlerContext.mCommandHandler.AddStatus(handlerContext.mRequestPath, Status::Success);

        break;
    }
    default:
        handlerContext.mCommandHandler.AddStatus(handlerContext.mRequestPath, Status::UnsupportedCommand);
        mLogger.warning("unsupported window covering command: " ChipLogFormatMEI " on endpoint: %u", ChipLogValueMEI(handlerContext.mRequestPath.mCommandId), handlerContext.mRequestPath.mEndpointId);
    }
}

}
