#include "Switch.h"
#include "SwitchEvents.h"

#include <csignal>
#include <memory>

using namespace mobmatter::common::domain;

namespace mobmatter::application::model {

Switch Switch::add(EndpointId endpointId, MobilusDeviceId mobilusDeviceId, State state, std::string name)
{
    Switch newSwitch(endpointId, mobilusDeviceId, state, std::move(name));
    newSwitch.raise(std::make_unique<SwitchAdded>(endpointId, mobilusDeviceId));

    return newSwitch;
}

Switch Switch::restoreFrom(EndpointId endpointId, MobilusDeviceId mobilusDeviceId, State state, std::string name)
{
    return { endpointId, mobilusDeviceId, state, std::move(name) };
}

Switch::Switch(EndpointId endpointId, MobilusDeviceId mobilusDeviceId, State state, std::string name)
    : Device(endpointId, mobilusDeviceId, std::move(name))
    , mState(state)
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
    switch (mState) {
    case State::On:
        return requestOff();
    case State::Off:
        return requestOn();
    case State::Unreachable:
    default:
        return Result::NoChange;
    }
}

Switch::Result Switch::reportOn()
{
    return turnOn();
}

Switch::Result Switch::reportOff()
{
    return turnOff();
}

Switch::Result Switch::reportError(Error error)
{
    switch (error) {
    case Error::Unreachable:
        if (mState == State::Unreachable) {
            return Result::NoChange;
        }

        mState = State::Unreachable;
        raise(std::make_unique<SwitchMarkedAsUnreachable>(mEndpointId, mMobilusDeviceId));

        return Result::Ok;
    case Error::Unknown:
    default:
        return Result::NoChange;
    }
}

Switch::Result Switch::turnOn()
{
    if (mState == State::On) {
        return Result::NoChange;
    }

    mState = State::On;
    raise(std::make_unique<SwitchTurnedOn>(mEndpointId, mMobilusDeviceId));

    return Result::Ok;
}

Switch::Result Switch::turnOff()
{
    if (mState == State::Off) {
        return Result::NoChange;
    }

    mState = State::Off;
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
