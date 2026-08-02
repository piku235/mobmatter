#pragma once

#include "application/model/MobilusDeviceId.h"

namespace mobmatter::application::driven_ports {

class SwitchControlService {
public:
    virtual ~SwitchControlService() = default;
    virtual void turnSwitchOn(model::MobilusDeviceId deviceId) = 0;
    virtual void turnSwitchOff(model::MobilusDeviceId deviceId) = 0;
};

}
