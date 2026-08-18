#pragma once

#include <lib/core/DataModelTypes.h>

#include <string>

namespace mobmatter::driving_adapters::matter::bridged_device_cluster {

struct BridgedDeviceBasicInfoData final {
    chip::EndpointId endpointId;
    std::string uniqueId;
    bool reachable;
    std::string productName;
    std::string nodeLabel;
};

}
