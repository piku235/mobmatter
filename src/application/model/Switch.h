#pragma once

#include "application/model/Device.h"

namespace mobmatter::application::model {

class Switch final : public Device {
public:
    enum class Error {
        Unknown,
        Unreachable,
    };
    enum class State {
        On,
        Off,
        Unreachable,
    };

    static Switch add(EndpointId endpointId, MobilusDeviceId mobilusDeviceId, State state, std::string name);
    static Switch restoreFrom(EndpointId endpointId, MobilusDeviceId mobilusDeviceId, State state, std::string name);

    /* chip oriented */
    Result requestOn();
    Result requestOff();
    Result requestToggle();

    /* mobilus oriented */
    Result reportOn();
    Result reportOff();
    Result reportError(Error error);

    bool isReachable() const { return mState != State::Unreachable; }
    State state() const { return mState; }

private:
    State mState;

    Switch(EndpointId endpointId, MobilusDeviceId mobilusDeviceId, State state, std::string name);
    Result turnOn();
    Result turnOff();

    std::unique_ptr<common::domain::DomainEvent> deviceRemoved() override;
    std::unique_ptr<common::domain::DomainEvent> deviceRenamed() override;
};

}
