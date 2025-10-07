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

        cover->startLiftTo(*position);
        mCoverRepository.save(*cover);

        break;
    }
    case EventNumber::Reached: {
        auto position = ConversionUtils::convertLiftPosition(event.value());

        if (!position) {
            mLogger.error(LOG_TAG "Invalid cover lift position: %s" LOG_SUFFIX, event.value().c_str(), cover->endpointId(), cover->mobilusDeviceId());
            break;
        }

        if (auto e = cover->changeLiftPosition(*position); !e) {
            mLogger.error(LOG_TAG "Failed to change lift position: %s" LOG_SUFFIX, event.value().c_str(), cover->endpointId(), cover->mobilusDeviceId());
            break;
        }

        mCoverRepository.save(*cover);

        break;
    }
    case EventNumber::Error:
        mLogger.notice(LOG_TAG "Device error: %s" LOG_SUFFIX, event.value().c_str(), cover->endpointId(), cover->mobilusDeviceId());

        if ("NO_CONNECTION" == event.value()) {
            cover->markAsUnreachable();
            mCoverRepository.save(*cover);

            break;
        }

        cover->failMotion();
        mCoverRepository.save(*cover);

        break;
    default:
        mLogger.notice(LOG_TAG "Unknown event number");
    }

    return Result::Handled;
}

}
