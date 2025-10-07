#include "MobilusCoverEventHandler.h"
#include "driving_adapters/mobilus/ConversionUtils.h"
#include "driving_adapters/mobilus/Log.h"
#include "jungi/mobilus_gtw_client/EventNumber.h"

using namespace jungi::mobilus_gtw_client;
using mobmatter::application::driven_ports::CoverRepository;

namespace mobmatter::driving_adapters::mobilus::cover {

MobilusCoverEventHandler::MobilusCoverEventHandler(CoverRepository& coverRepository, logging::Logger& logger)
    : mCoverRepository(coverRepository)
    , mLogger(logger)
{
}

MobilusCoverEventHandler::Result MobilusCoverEventHandler::handle(const proto::Event& event)
{
    if (!event.has_device_id()) {
        return Result::Handled; // ignore
    }

    auto cover = mCoverRepository.findOfMobilusDeviceId(event.device_id());

    if (!cover) {
        return Result::UnmatchedDevice;
    }

    switch (event.event_number()) {
    case EventNumber::Device:
        if ("REMOVE" == event.value()) {
            mCoverRepository.remove(*cover);
            mLogger.notice(LOG_TAG "Removed cover" LOG_SUFFIX, cover->endpointId(), cover->mobilusDeviceId());
        }

        // todo: ADD is bugged, missing device id
        break;
    case EventNumber::Sent: {
        auto position = ConversionUtils::convertLiftPosition(event.value());

        if (!position) {
            if ("STOP" == event.value()) {
                cover->initiateStopMotion();
                mCoverRepository.save(*cover);

                break;
            }

            mLogger.error(LOG_TAG "Invalid cover lift position: %s" LOG_SUFFIX, event.value().c_str(), cover->endpointId(), cover->mobilusDeviceId());
            break;
        }

        if (auto e = cover->startLiftTo(*position); !e) {
            mLogger.error(LOG_TAG "Failed to lift cover to target position: %d%%" LOG_SUFFIX, position->closedPercent().value(), cover->endpointId(), cover->mobilusDeviceId());
            break;
        }

        mCoverRepository.save(*cover);
        mLogger.notice(LOG_TAG "Started lifting cover to target position: %d%%" LOG_SUFFIX, position->closedPercent().value(), cover->endpointId(), cover->mobilusDeviceId());

        break;
    }
    case EventNumber::Reached: {
        auto position = ConversionUtils::convertLiftPosition(event.value());

        if (!position) {
            mLogger.error(LOG_TAG "Invalid cover lift position: %s" LOG_SUFFIX, event.value().c_str(), cover->endpointId(), cover->mobilusDeviceId());
            break;
        }

        if (auto e = cover->changeLiftPosition(*position); !e) {
            mLogger.error(LOG_TAG "Failed to change cover lift position: %d%%" LOG_SUFFIX, position->closedPercent().value(), cover->endpointId(), cover->mobilusDeviceId());
            break;
        }

        mCoverRepository.save(*cover);
        mLogger.notice(LOG_TAG "Changed cover lift position: %d%%" LOG_SUFFIX, position->closedPercent().value(), cover->endpointId(), cover->mobilusDeviceId());

        break;
    }
    case EventNumber::Error:
        if ("NO_CONNECTION" == event.value()) {
            cover->markAsUnreachable();
            mCoverRepository.save(*cover);
            mLogger.notice(LOG_TAG "Cover marked as unreachable" LOG_SUFFIX, cover->endpointId(), cover->mobilusDeviceId());

            break;
        }

        cover->failMotion();
        mCoverRepository.save(*cover);
        mLogger.notice(LOG_TAG "Cover motion failed: %s" LOG_SUFFIX, event.value().c_str(), cover->endpointId(), cover->mobilusDeviceId());

        break;
    default:
        mLogger.notice(LOG_TAG "Unknown event number");
    }

    return Result::Handled;
}

}
