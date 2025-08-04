#include "MqttMobilusCoverControlService.h"
#include "jungi/mobilus_gtw_client/EventNumber.h"
#include "jungi/mobilus_gtw_client/Platform.h"
#include "jungi/mobilus_gtw_client/proto/CallEvents.pb.h"

#include <cinttypes>

using namespace jungi::mobilus_gtw_client;
using namespace mmbridge::application::model;
using namespace mmbridge::application::model::window_covering;

namespace mmbridge::driven_adapters::mobilus::cover_control {

MqttMobilusCoverControlService::MqttMobilusCoverControlService(MqttMobilusGtwClient& mobilusGtwClient, logging::Logger& logger)
    : mMobilusGtwClient(mobilusGtwClient)
    , mLogger(logger)
{
}

void MqttMobilusCoverControlService::liftCover(MobilusDeviceId mobilusDeviceId, Position position)
{
    auto e = mMobilusGtwClient.send(callEventsFor(mobilusDeviceId, convertLiftPosition(position)));
    
    if (e) {
        mLogger.info("MQTT lift command sent to cover [md=%" PRId64 "]", mobilusDeviceId);
        return;
    }
    
    mLogger.error("MQTT lift command failed for cover: %s [md=%" PRId64 "]", e.error().message.c_str(), mobilusDeviceId);
}

void MqttMobilusCoverControlService::stopCoverMotion(MobilusDeviceId mobilusDeviceId)
{
    auto e = mMobilusGtwClient.send(callEventsFor(mobilusDeviceId, "STOP"));
    
    if (e) {
        mLogger.info("MQTT stop motion command sent to cover [md=%" PRId64 "]", mobilusDeviceId);
        return;
    }

    mLogger.error("MQTT stop motion command failed for cover: %s [md=%" PRId64 "]", e.error().message.c_str(), mobilusDeviceId);
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
