#pragma once

#include "DeviceStateMap.h"

namespace mobmatter::driving_adapters::mobilus {

class MobilusDeviceSyncHandler {
public:
    virtual ~MobilusDeviceSyncHandler() = default;
    virtual void sync(const DeviceStateMap& devices) = 0;
};

}
