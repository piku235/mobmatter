#pragma once

#include "BridgedDeviceBasicInfoData.h"

#include <lib/core/DataModelTypes.h>

#include <optional>

namespace mobmatter::driving_adapters::matter::bridged_device_cluster {

class BridgedDeviceBasicInfoLoader {
public:
    virtual ~BridgedDeviceBasicInfoLoader() = default;
    virtual std::optional<BridgedDeviceBasicInfoData> load(chip::EndpointId endpointId) const = 0;
};

}
