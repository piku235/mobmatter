#pragma once

#include "application/driven_ports/SwitchRepository.h"

#include <app/AttributeAccessInterface.h>

namespace mobmatter::driving_adapters::matter::switch_cluster {

namespace driven_ports = application::driven_ports;

class BridgedDeviceBasicInfoAttributeAccess final : public chip::app::AttributeAccessInterface {
public:
    explicit BridgedDeviceBasicInfoAttributeAccess(chip::EndpointId endpointId, driven_ports::SwitchRepository& switchRepository);

    CHIP_ERROR Read(const chip::app::ConcreteReadAttributePath& path, chip::app::AttributeValueEncoder& encoder) override;
    CHIP_ERROR Write(const chip::app::ConcreteDataAttributePath& path, chip::app::AttributeValueDecoder& decoder) override;

private:
    driven_ports::SwitchRepository& mSwitchRepository;
};

}
