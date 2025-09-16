#include "MqttMobilusCoverControlService.h"
#include "jungi/mobilus_gtw_client/EventNumber.h"
#include "jungi/mobilus_gtw_client/Platform.h"
#include "jungi/mobilus_gtw_client/proto/CallEvents.pb.h"

#include <cinttypes>

#define LOG_TAG "GTW: "

using namespace jungi::mobilus_gtw_client;
using namespace mobmatter::application::model;
using namespace mobmatter::application::model::window_covering;

namespace mobmatter::driven_adapters::mobilus {

MqttMobilusCoverControlService::MqttMobilusCoverControlService(MqttMobilusGtwClient& mobilusGtwClient, logging::Logger& logger)
    : mMobilusGtwClient(mobilusGtwClient)
    , mLogger(logger)
{
}

void MqttMobilusCoverControlService::liftCover(MobilusDeviceId mobilusDeviceId, Position position)
{
    if (mMobilusGtwClient.send(callEventsFor(mobilusDeviceId, convertLiftPosition(position)))) {
        mLogger.info(LOG_TAG "Lift command sent to cover [md=%" PRId64 "]", mobilusDeviceId);
        return;
    }

    mLogger.error(LOG_TAG "Lift command failed for cover [md=%" PRId64 "]", mobilusDeviceId);
}

void MqttMobilusCoverControlService::stopCoverMotion(MobilusDeviceId mobilusDeviceId)
{
    if (mMobilusGtwClient.send(callEventsFor(mobilusDeviceId, "STOP"))) {
        mLogger.info(LOG_TAG "Stop motion command sent to cover [md=%" PRId64 "]", mobilusDeviceId);
        return;
    }

    mLogger.error(LOG_TAG "Stop motion command failed for cover [md=%" PRId64 "]", mobilusDeviceId);
}

proto::CallEvents MqttMobilusCoverControlService::callEventsFor(MobilusDeviceId mobilusDeviceId, const std::string& eventValue) const
{
    proto::CallEvents callEvents;
    auto event = callEvents.add_events();

    event->set_device_id(mobilusDeviceId);
    event->set_event_number(static_cast<int32_t>(EventNumber::Triggered));
    event->set_value(eventValue);
    event->set_platform(static_cast<int32_t>(Platform::Web));

    return callEvents;
}

std::string MqttMobilusCoverControlService::convertLiftPosition(Position position) const
{
    if (position.isFullyOpen()) {
        return "UP";
    }

    if (position.isFullyClosed()) {
        return "DOWN";
    }

    return std::to_string(position.openPercent().value()) + "%";
}

}
