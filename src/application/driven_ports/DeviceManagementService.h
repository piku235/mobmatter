#pragma once

#include "application/model/MobilusDeviceId.h"

#include <string>

namespace mobmatter::application::driven_ports {

namespace model = mobmatter::application::model;

class DeviceManagementService {
public:
    virtual ~DeviceManagementService() = default;

    virtual void renameDevice(model::MobilusDeviceId deviceId, std::string name) = 0;
};

}
