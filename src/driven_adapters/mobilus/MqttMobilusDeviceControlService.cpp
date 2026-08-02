#include "MqttMobilusDeviceControlService.h"

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

MqttMobilusDeviceControlService::MqttMobilusDeviceControlService(MqttMobilusGtwClient& client, logging::Logger& logger)
    : mClient(client)
    , mLogger(logger)
{
}

void MqttMobilusDeviceControlService::openCover(MobilusDeviceId deviceId)
{
    if (mClient.send(callEventsFor(deviceId, "UP"))) {
        mLogger.info(LOG_TAG "Open command sent to cover" LOG_SUFFIX, deviceId);
        return;
    }

    mLogger.error(LOG_TAG "Open command failed for cover" LOG_SUFFIX, deviceId);
}

void MqttMobilusDeviceControlService::closeCover(MobilusDeviceId deviceId)
{
    if (mClient.send(callEventsFor(deviceId, "DOWN"))) {
        mLogger.info(LOG_TAG "Close command sent to cover" LOG_SUFFIX, deviceId);
        return;
    }

    mLogger.error(LOG_TAG "Close command failed for cover" LOG_SUFFIX, deviceId);
}

void MqttMobilusDeviceControlService::liftCover(MobilusDeviceId deviceId, Position position)
{
    if (mClient.send(callEventsFor(deviceId, convertLiftPosition(position)))) {
        mLogger.info(LOG_TAG "Lift command sent to cover" LOG_SUFFIX, deviceId);
        return;
    }

    mLogger.error(LOG_TAG "Lift command failed for cover" LOG_SUFFIX, deviceId);
}

void MqttMobilusDeviceControlService::tiltCover(MobilusDeviceId deviceId, Position position)
{
    if (mClient.send(callEventsFor(deviceId, convertTiltPosition(position)))) {
        mLogger.info(LOG_TAG "Tilt command sent to cover" LOG_SUFFIX, deviceId);
        return;
    }

    mLogger.error(LOG_TAG "Tilt command failed for cover" LOG_SUFFIX, deviceId);
}

void MqttMobilusDeviceControlService::stopCoverMotion(MobilusDeviceId deviceId)
{
    if (mClient.send(callEventsFor(deviceId, "STOP"))) {
        mLogger.info(LOG_TAG "Stop motion command sent to cover" LOG_SUFFIX, deviceId);
        return;
    }

    mLogger.error(LOG_TAG "Stop motion command failed for cover" LOG_SUFFIX, deviceId);
}

void MqttMobilusDeviceControlService::turnSwitchOn(MobilusDeviceId deviceId)
{
    if (mClient.send(callEventsFor(deviceId, "ON"))) {
        mLogger.info(LOG_TAG "Turn switch on command sent" LOG_SUFFIX, deviceId);
        return;
    }

    mLogger.error(LOG_TAG "Turn switch on command failed" LOG_SUFFIX, deviceId);
}

void MqttMobilusDeviceControlService::turnSwitchOff(MobilusDeviceId deviceId)
{
    if (mClient.send(callEventsFor(deviceId, "OFF"))) {
        mLogger.info(LOG_TAG "Turn switch off command sent" LOG_SUFFIX, deviceId);
        return;
    }

    mLogger.error(LOG_TAG "Turn switch off command failed" LOG_SUFFIX, deviceId);
}

proto::CallEvents MqttMobilusDeviceControlService::callEventsFor(MobilusDeviceId deviceId, const std::string& eventValue)
{
    proto::CallEvents callEvents;
    auto event = callEvents.add_events();

    event->set_device_id(deviceId);
    event->set_event_number(EventNumber::Triggered);
    event->set_value(eventValue);
    event->set_platform(Platform::Web);

    return callEvents;
}

std::string MqttMobilusDeviceControlService::convertLiftPosition(Position position)
{
    return std::to_string(position.openPercent().value()) + "%";
}

std::string MqttMobilusDeviceControlService::convertTiltPosition(Position position)
{
    return std::to_string(position.openPercent().value()) + "$";
}

}
