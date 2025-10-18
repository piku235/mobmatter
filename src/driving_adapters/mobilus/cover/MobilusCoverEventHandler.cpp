#include "MobilusCoverEventHandler.h"
#include "application/model/window_covering/Cover.h"
#include "driving_adapters/mobilus/ConversionUtils.h"
#include "driving_adapters/mobilus/Log.h"
#include "jungi/mobilus_gtw_client/EventNumber.h"

using namespace jungi::mobilus_gtw_client;
using mobmatter::application::model::window_covering::Cover;

namespace mobmatter::driving_adapters::mobilus::cover {

MobilusCoverEventHandler::MobilusCoverEventHandler(driven_ports::CoverRepository& coverRepository, logging::Logger& logger)
    : mCoverRepository(coverRepository)
    , mLogger(logger)
{
}

MobilusCoverEventHandler::Result MobilusCoverEventHandler::handle(const proto::Event& event)
{
    if (!event.has_device_id()) {
        return Result::Unsupported;
    }

    auto cover = mCoverRepository.findOfMobilusDeviceId(event.device_id());

    if (!cover) {
        return Result::Unsupported;
    }

    switch (event.event_number()) {
    case EventNumber::Device:
        if ("REMOVE" == event.value()) {
            cover->remove();
            mCoverRepository.remove(*cover);

            mLogger.notice(LOG_TAG "Removed cover" LOG_SUFFIX, cover->endpointId(), cover->mobilusDeviceId());
        }

        // todo: ADD is bugged, missing device id
        break;
    case EventNumber::Sent: {
        auto position = ConversionUtils::convertLiftPosition(event.value());

        if (!position) {
            if ("STOP" == event.value()) {
                if (Cover::Result::Ok == cover->initiateStopMotion()) {
                    mCoverRepository.save(*cover);
                }

                break;
            }

            mLogger.error(LOG_TAG "Invalid cover lift position: %s" LOG_SUFFIX, event.value().c_str(), cover->endpointId(), cover->mobilusDeviceId());
            break;
        }

        if (Cover::Result::Ok == cover->startLiftTo(*position)) {
            mCoverRepository.save(*cover);
            mLogger.notice(LOG_TAG "Started lifting cover to target position: %d%%" LOG_SUFFIX, position->closedPercent().value(), cover->endpointId(), cover->mobilusDeviceId());
        }

        break;
    }
    case EventNumber::Reached: {
        auto position = ConversionUtils::convertLiftPosition(event.value());

        if (!position) {
            mLogger.error(LOG_TAG "Invalid cover lift position: %s" LOG_SUFFIX, event.value().c_str(), cover->endpointId(), cover->mobilusDeviceId());
            break;
        }

        if (Cover::Result::Ok == cover->changeLiftPosition(*position)) {
            mCoverRepository.save(*cover);
            mLogger.notice(LOG_TAG "Changed cover lift position: %d%%" LOG_SUFFIX, position->closedPercent().value(), cover->endpointId(), cover->mobilusDeviceId());
        }

        break;
    }
    case EventNumber::Error:
        if ("NO_CONNECTION" == event.value()) {
            if (Cover::Result::Ok == cover->markAsUnreachable()) {
                mCoverRepository.save(*cover);
                mLogger.notice(LOG_TAG "Cover marked as unreachable" LOG_SUFFIX, cover->endpointId(), cover->mobilusDeviceId());
            }

            break;
        }

        if (Cover::Result::Ok == cover->failMotion()) {
            mCoverRepository.save(*cover);
            mLogger.notice(LOG_TAG "Cover motion failed: %s" LOG_SUFFIX, event.value().c_str(), cover->endpointId(), cover->mobilusDeviceId());
        }

        break;
    default:
        mLogger.notice(LOG_TAG "Unknown event number");
    }

    return Result::Handled;
}

}
