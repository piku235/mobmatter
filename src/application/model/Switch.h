#pragma once

#include "application/model/Device.h"

namespace mobmatter::application::model {

class Switch final : public Device {
public:
    [[nodiscard]] static Switch add(EndpointId endpointId, MobilusDeviceId mobilusDeviceId, bool onOff, std::string name);
    [[nodiscard]] static Switch restoreFrom(EndpointId endpointId, MobilusDeviceId mobilusDeviceId, bool reachable, bool onOff, std::string name);

    /* chip oriented */
    Result requestOn();
    Result requestOff();
    Result requestToggle();

    /* mobilus oriented */
    Result reportOn();
    Result reportOff();
    Result reportReachable();
    Result reportUnreachable();

    [[nodiscard]] bool isReachable() const { return mReachable; }
    [[nodiscard]] bool isOn() const { return mOnOff; }

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
