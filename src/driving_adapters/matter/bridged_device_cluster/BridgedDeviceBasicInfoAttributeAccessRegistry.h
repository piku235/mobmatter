#pragma once

#include "BridgedDeviceBasicInfoAttributeAccess.h"

#include <optional>

namespace mobmatter::driving_adapters::matter::bridged_device_cluster {

class BridgedDeviceBasicInfoAttributeAccessRegistry final {
public:
    void registerAttributeAccess(chip::EndpointId endpointId, BridgedDeviceBasicInfoLoader& loader);
    void unregisterAttributeAccess(chip::EndpointId endpointId);

private:
    std::optional<BridgedDeviceBasicInfoAttributeAccess> mAttributeAccessList[CHIP_DEVICE_CONFIG_DYNAMIC_ENDPOINT_COUNT];
};

}
