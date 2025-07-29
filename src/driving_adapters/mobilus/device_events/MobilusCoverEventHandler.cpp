#include "MobilusCoverEventHandler.h"
#include "driving_adapters/mobilus/ConversionUtils.h"
#include "jungi/mobilus_gtw_client/EventNumber.h"

using namespace jungi::mobilus_gtw_client;
using mmbridge::application::driven_ports::CoverRepository;

namespace mmbridge::driving_adapters::mobilus::device_events {

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
        if (!event.value().compare("REMOVE")) {
            mCoverRepository.remove(*cover);
        }

        // todo: ADD is bugged, missing device id
        break;
    case EventNumber::Sent: {
        auto position = ConversionUtils::convertLiftPosition(event.value());

        if (!position) {
            if (!event.value().compare("STOP")) {
                cover->initiateStopMotion();
                mCoverRepository.save(*cover);

                break;
            }

            mLogger.error("MQTT invalid cover lift position: %s", event.value().c_str());
            break;
        }

        cover->startLiftTo(*position);
        mCoverRepository.save(*cover);

        break;
    }
    case EventNumber::Reached: {
        auto position = ConversionUtils::convertLiftPosition(event.value());

        if (!position) {
            mLogger.error("MQTT invalid cover lift position: %s", event.value().c_str());
            break;
        }

        cover->changeLiftPosition(*position);
        mCoverRepository.save(*cover);

        break;
    }
    case EventNumber::Error:
        mLogger.notice("MQTT device error: %s", event.value().c_str());

        if (!event.value().compare("NO_CONNECTION")) {
            cover->markAsUnreachable();
            mCoverRepository.save(*cover);

            break;
        }

        cover->failMotion();
        mCoverRepository.save(*cover);

        break;
    default:
        mLogger.notice("MQTT unknown event number");
    }

    return Result::Handled;
}

}
