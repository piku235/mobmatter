#pragma once

#include "DeviceState.h"
#include "application/model/MobilusDeviceId.h"

#include <unordered_map>

namespace mobmatter::driving_adapters::mobilus {

namespace model = mobmatter::application::model;

using DeviceStateMap = std::unordered_map<model::MobilusDeviceId, DeviceState>;

}
