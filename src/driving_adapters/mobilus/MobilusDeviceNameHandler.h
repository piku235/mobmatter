#pragma once

#include "application/model/MobilusDeviceId.h"
#include "application/model/MobilusDeviceType.h"

#include <string>

namespace mobmatter::driving_adapters::mobilus {

namespace model = mobmatter::application::model;

class MobilusDeviceNameHandler {
public:
    virtual ~MobilusDeviceNameHandler() = default;

    virtual void handle(model::MobilusDeviceId deviceId, const std::string& name) = 0;
    virtual bool supports(model::MobilusDeviceType deviceType) const = 0;
};

}
