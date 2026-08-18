#pragma once

#include "application/driven_ports/CoverRepository.h"

#include <app/AttributeAccessInterface.h>

namespace mobmatter::driving_adapters::matter::cover_cluster {

namespace driven_ports = application::driven_ports;

class BridgedDeviceBasicInfoAttributeAccess final : public chip::app::AttributeAccessInterface {
public:
    explicit BridgedDeviceBasicInfoAttributeAccess(chip::EndpointId endpointId, driven_ports::CoverRepository& coverRepository);
    CHIP_ERROR Read(const chip::app::ConcreteReadAttributePath& path, chip::app::AttributeValueEncoder& encoder) override;

private:
    driven_ports::CoverRepository& mCoverRepository;
};

}
