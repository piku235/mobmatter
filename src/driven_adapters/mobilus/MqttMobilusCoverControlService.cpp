#include "MqttMobilusCoverControlService.h"

#include <jungi/mobgtw/EventNumber.h>
#include <jungi/mobgtw/Platform.h>
#include <jungi/mobgtw/proto/CallEvents.pb.h>

#include <cinttypes>

#define LOG_TAG "GTW: "
#define LOG_SUFFIX " [md=%" PRId64 "]"

using namespace jungi::mobgtw;
using namespace mobmatter::application::model;
using namespace mobmatter::application::model::window_covering;

namespace mobmatter::driven_adapters::mobilus {

MqttMobilusCoverControlService::MqttMobilusCoverControlService(MqttMobilusGtwClient& client, logging::Logger& logger)
    : mClient(client)
    , mLogger(logger)
{
}

void MqttMobilusCoverControlService::liftCover(MobilusDeviceId deviceId, Position position)
{
    if (mClient.send(callEventsFor(deviceId, convertLiftPosition(position)))) {
        mLogger.info(LOG_TAG "Lift command sent to cover" LOG_SUFFIX, deviceId);
        return;
    }

    mLogger.error(LOG_TAG "Lift command failed for cover" LOG_SUFFIX, deviceId);
}

void MqttMobilusCoverControlService::stopCoverMotion(MobilusDeviceId deviceId)
{
    if (mClient.send(callEventsFor(deviceId, "STOP"))) {
        mLogger.info(LOG_TAG "Stop motion command sent to cover" LOG_SUFFIX, deviceId);
        return;
    }

    mLogger.error(LOG_TAG "Stop motion command failed for cover" LOG_SUFFIX, deviceId);
}

proto::CallEvents MqttMobilusCoverControlService::callEventsFor(MobilusDeviceId deviceId, const std::string& eventValue) const
{
    proto::CallEvents callEvents;
    auto event = callEvents.add_events();

    event->set_device_id(deviceId);
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
