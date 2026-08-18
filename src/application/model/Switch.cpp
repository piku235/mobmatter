#include "Switch.h"
#include "SwitchEvents.h"

#include <csignal>
#include <memory>

using namespace mobmatter::common::domain;

namespace mobmatter::application::model {

Switch Switch::add(EndpointId endpointId, MobilusDeviceId mobilusDeviceId, bool onOff, std::string name)
{
    Switch newSwitch(endpointId, mobilusDeviceId, true, onOff, std::move(name));
    newSwitch.raise(std::make_unique<SwitchAdded>(endpointId, mobilusDeviceId));

    return newSwitch;
}

Switch Switch::restoreFrom(EndpointId endpointId, MobilusDeviceId mobilusDeviceId, bool reachable, bool onOff, std::string name)
{
    return { endpointId, mobilusDeviceId, reachable, onOff, std::move(name) };
}

Switch::Switch(EndpointId endpointId, MobilusDeviceId mobilusDeviceId, bool reachable, bool onOff, std::string name)
    : Device(endpointId, mobilusDeviceId, std::move(name))
    , mReachable(reachable)
    , mOnOff(onOff)
{
}

Switch::Result Switch::requestOn()
{
    auto result = turnOn();

    if (Result::Ok == result) {
        raise(std::make_unique<SwitchTurnOnRequested>(mEndpointId, mMobilusDeviceId));
    }

    return result;
}

Switch::Result Switch::requestOff()
{
    auto result = turnOff();

    if (Result::Ok == result) {
        raise(std::make_unique<SwitchTurnOffRequested>(mEndpointId, mMobilusDeviceId));
    }

    return result;
}

Switch::Result Switch::requestToggle()
{
    if (mOnOff) {
        return requestOff();
    }

    return requestOn();
}

Switch::Result Switch::reportOn()
{
    return turnOn();
}

Switch::Result Switch::reportOff()
{
    return turnOff();
}

Switch::Result Switch::reportReachable()
{
    if (mReachable) {
        return Result::NoChange;
    }

    mReachable = true;
    raise(std::make_unique<SwitchMarkedAsReachable>(mEndpointId, mMobilusDeviceId));

    return Result::Ok;
}

Switch::Result Switch::reportError(Error error)
{
    switch (error) {
    case Error::Unreachable:
        if (!mReachable) {
            return Result::NoChange;
        }

        mReachable = false;
        raise(std::make_unique<SwitchMarkedAsUnreachable>(mEndpointId, mMobilusDeviceId));

        return Result::Ok;
    case Error::Unknown:
    default:
        return Result::NoChange;
    }
}

Switch::Result Switch::turnOn()
{
    if (mOnOff) {
        return Result::NoChange;
    }

    mOnOff = true;
    raise(std::make_unique<SwitchTurnedOn>(mEndpointId, mMobilusDeviceId));

    return Result::Ok;
}

Switch::Result Switch::turnOff()
{
    if (!mOnOff) {
        return Result::NoChange;
    }

    mOnOff = false;
    raise(std::make_unique<SwitchTurnedOff>(mEndpointId, mMobilusDeviceId));

    return Result::Ok;
}

std::unique_ptr<DomainEvent> Switch::deviceRemoved()
{
    return std::make_unique<SwitchRemoved>(mEndpointId, mMobilusDeviceId);
}

std::unique_ptr<DomainEvent> Switch::deviceRenamed()
{
    return std::make_unique<SwitchRenamed>(mEndpointId, mMobilusDeviceId, mName);
}

}
