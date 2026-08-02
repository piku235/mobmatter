#include "MobilusSwitchHandler.h"

#include "application/model/MobilusDeviceType.h"
#include "application/model/Switch.h"
#include "driving_adapters/mobilus/Log.h"

#include <cinttypes>
#include <jungi/mobgtw/EventNumber.h>

#define LOG_SUFFIX " [md=%" PRId64 "]"
#define LOG_SUFFIX_EP " [ep=%u, md=%" PRId64 "]"

using namespace jungi::mobgtw;
using namespace mobmatter::application::model;

namespace mobmatter::driving_adapters::mobilus::device_handlers {

MobilusSwitchHandler::MobilusSwitchHandler(driven_ports::SwitchRepository& switchRepository, driven_ports::EndpointIdGenerator& endpointIdGenerator, logging::Logger& logger)
    : mSwitchRepository(switchRepository)
    , mEndpointIdGenerator(endpointIdGenerator)
    , mLogger(logger)
{
}

void MobilusSwitchHandler::sync(const DeviceStateMap& devices)
{
    // non-existing
    for (auto& cover : mSwitchRepository.all()) {
        if (devices.end() == devices.find(cover.mobilusDeviceId())) {
            cover.reportRemoved();
            mSwitchRepository.remove(cover);

            mLogger.notice(LOG_TAG "Removed switch" LOG_SUFFIX_EP, cover.endpointId(), cover.mobilusDeviceId());
        }
    }

    // existing
    for (auto& [deviceId, deviceState] : devices) {
        if (deviceState.device.type() != static_cast<int>(MobilusDeviceType::Switch)
            && deviceState.device.type() != static_cast<int>(MobilusDeviceType::SwitchNp)) {
            continue;
        }

        auto switch_ = mSwitchRepository.findOfMobilusDeviceId(deviceId);

        if (switch_) {
            if (apply(*switch_, deviceState.device) | apply(*switch_, deviceState.lastEvent)) {
                mSwitchRepository.save(*switch_);
            }

            continue;
        }

        init(deviceState.device, deviceState.lastEvent);
    }
}

MobilusDeviceEventHandler::Result MobilusSwitchHandler::handle(const proto::Event& event)
{
    if (!event.has_device_id()) {
        return Result::Unmatched;
    }

    auto switch_ = mSwitchRepository.findOfMobilusDeviceId(event.device_id());

    if (!switch_) {
        return Result::Unmatched;
    }

    if (apply(*switch_, event)) {
        mSwitchRepository.save(*switch_);
    }

    return Result::Handled;
}

void MobilusSwitchHandler::init(const proto::Device& device, const proto::Event& lastEvent)
{
    Switch::State state = Switch::State::Unreachable;

    switch (lastEvent.event_number()) {
    case EventNumber::Sent:
    case EventNumber::Reached:
        if ("ON" == lastEvent.value()) {
            state = Switch::State::On;
            break;
        }
        if ("OFF" == lastEvent.value()) {
            state = Switch::State::Off;
            break;
        }

        break;
    case EventNumber::Error:
        if ("NO_CONNECTION" == lastEvent.value()) {
            state = Switch::State::Unreachable;
            break;
        }

        break;
    }

    auto endpointId = mEndpointIdGenerator.next();

    if (!endpointId) {
        mLogger.error(LOG_TAG "Could not get next endpoint id" LOG_SUFFIX, device.id());
        return;
    }

    auto newSwitch = Switch::add(*endpointId, device.id(), device.name(), state);
    mSwitchRepository.save(newSwitch);

    mLogger.notice(LOG_TAG "Added switch" LOG_SUFFIX_EP, newSwitch.endpointId(), device.id());
}

bool MobilusSwitchHandler::apply(Switch& switch_, const proto::Device& deviceInfo)
{
    if (Switch::Result::Ok == switch_.reportRenamedTo(deviceInfo.name())) {
        mLogger.notice(LOG_TAG "Renamed switch to: %s" LOG_SUFFIX_EP, deviceInfo.name().c_str(), switch_.endpointId(), switch_.mobilusDeviceId());
        return true;
    }

    return false;
}

bool MobilusSwitchHandler::apply(Switch& switch_, const proto::Event& event)
{
    switch (event.event_number()) {
    case EventNumber::Sent:
    case EventNumber::Reached: {
        if ("ON" == event.value() && Switch::Result::Ok == switch_.reportOn()) {
            mLogger.notice(LOG_TAG "Turned switch on" LOG_SUFFIX_EP, switch_.endpointId(), switch_.mobilusDeviceId());
            return true;
        }
        if ("OFF" == event.value() && Switch::Result::Ok == switch_.reportOff()) {
            mLogger.notice(LOG_TAG "Turned switch off" LOG_SUFFIX_EP, switch_.endpointId(), switch_.mobilusDeviceId());
            return true;
        }

        return false;
    }
    case EventNumber::Error: {
        auto error = parseError(event.value());

        if (!error) {
            mLogger.error(LOG_TAG "Unrecognized cover error: %s" LOG_SUFFIX_EP, event.value().c_str(), switch_.endpointId(), switch_.mobilusDeviceId());
            return false;
        }

        if (Switch::Result::Ok == switch_.reportError(*error)) {
            mLogger.notice(LOG_TAG "Cover reported error: %s" LOG_SUFFIX_EP, event.value().c_str(), switch_.endpointId(), switch_.mobilusDeviceId());
            return true;
        }

        return false;
    }
    default:
        mLogger.notice(LOG_TAG "Unknown event number");
        return false;
    }
}

std::optional<Switch::Error> MobilusSwitchHandler::parseError(const std::string& error)
{
    if ("UNKNOWN" == error) {
        return Switch::Error::Unknown;
    }
    if ("NO_CONNECTION" == error) {
        return Switch::Error::Unreachable;
    }

    return std::nullopt;
}

}
