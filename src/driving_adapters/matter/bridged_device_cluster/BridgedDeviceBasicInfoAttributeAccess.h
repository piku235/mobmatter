#pragma once

#include "BridgedDeviceBasicInfoLoader.h"

#include <app/AttributeAccessInterface.h>

namespace mobmatter::driving_adapters::matter::bridged_device_cluster {

class BridgedDeviceBasicInfoAttributeAccess final : public chip::app::AttributeAccessInterface {
public:
    explicit BridgedDeviceBasicInfoAttributeAccess(chip::EndpointId endpointId, BridgedDeviceBasicInfoLoader& loader);
    CHIP_ERROR Read(const chip::app::ConcreteReadAttributePath& path, chip::app::AttributeValueEncoder& encoder) override;

private:
    BridgedDeviceBasicInfoLoader& mLoader;
};

}
