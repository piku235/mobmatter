#include "Switch.h"
#include "SwitchEvents.h"

#include <csignal>
#include <memory>

namespace mobmatter::application::model {

Switch Switch::add(EndpointId endpointId, MobilusDeviceId mobilusDeviceId, std::string name, State state)
{
    Switch newSwitch(endpointId, mobilusDeviceId, UniqueId::random(), std::move(name), state);
    newSwitch.raise(std::make_unique<SwitchAdded>(endpointId, mobilusDeviceId));

    return newSwitch;
}

Switch Switch::restoreFrom(EndpointId endpointId, MobilusDeviceId mobilusDeviceId, UniqueId uniqueId, std::string name, State state)
{
    return { endpointId, mobilusDeviceId, uniqueId, std::move(name), state };
}

Switch::Switch(EndpointId endpointId, MobilusDeviceId mobilusDeviceId, UniqueId uniqueId, std::string name, State state)
    : mEndpointId(endpointId)
    , mMobilusDeviceId(mobilusDeviceId)
    , mUniqueId(uniqueId)
    , mName(std::move(name))
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

Switch::Result Switch::requestRename(std::string name)
{
    auto result = rename(std::move(name));

    if (Result::Ok == result) {
        raise(std::make_unique<SwitchRenameRequested>(mEndpointId, mMobilusDeviceId, mName));
    }

    return result;
}

Switch::Result Switch::reportOn()
{
    return turnOn();
}

Switch::Result Switch::reportOff()
{
    return turnOff();
}

Switch::Result Switch::reportRenamedTo(std::string name)
{
    return rename(std::move(name));
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

void Switch::reportRemoved()
{
    raise(std::make_unique<SwitchRemoved>(mMobilusDeviceId, mEndpointId));
}

bool Switch::operator==(const Switch& other) const
{
    return mEndpointId == other.mEndpointId;
}

EndpointId Switch::endpointId() const
{
    return mEndpointId;
}

MobilusDeviceId Switch::mobilusDeviceId() const
{
    return mMobilusDeviceId;
}

const UniqueId& Switch::uniqueId() const
{
    return mUniqueId;
}

const std::string& Switch::name() const
{
    return mName;
}

Switch::State Switch::state() const
{
    return mState;
}

Switch::Result Switch::rename(std::string name)
{
    if (mName == name) {
        return Result::NoChange;
    }

    mName = std::move(name);
    return Result::Ok;
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

}
