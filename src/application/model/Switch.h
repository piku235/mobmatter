#pragma once

#include "application/model/Device.h"

namespace mobmatter::application::model {

class Switch final : public Device {
public:
    enum class Error {
        Unknown,
        Unreachable,
    };

    static Switch add(EndpointId endpointId, MobilusDeviceId mobilusDeviceId, bool onOff, std::string name);
    static Switch restoreFrom(EndpointId endpointId, MobilusDeviceId mobilusDeviceId, bool reachable, bool onOff, std::string name);

    /* chip oriented */
    Result requestOn();
    Result requestOff();
    Result requestToggle();

    /* mobilus oriented */
    Result reportOn();
    Result reportOff();
    Result reportReachable();
    Result reportError(Error error);

    bool isReachable() const { return mReachable; }
    bool isOn() const { return mOnOff; }

private:
    bool mReachable;
    bool mOnOff;

    Switch(EndpointId endpointId, MobilusDeviceId mobilusDeviceId, bool reachable, bool onOff, std::string name);
    Result turnOn();
    Result turnOff();

    std::unique_ptr<common::domain::DomainEvent> deviceRemoved() override;
    std::unique_ptr<common::domain::DomainEvent> deviceRenamed() override;
};

}
