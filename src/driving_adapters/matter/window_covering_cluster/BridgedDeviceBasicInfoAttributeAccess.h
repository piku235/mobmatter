#pragma once

#include "application/driven_ports/CoverRepository.h"

#include <app/AttributeAccessInterface.h>

namespace mmbridge::driving_adapters::matter::window_covering_cluster {

namespace driven_ports = mmbridge::application::driven_ports;

/**
 * it would break if new devices were also handled
 * for now it is ok since window covering devices are only supported
 */
class BridgedDeviceBasicInfoAttributeAccess final : public chip::app::AttributeAccessInterface {
public:
    explicit BridgedDeviceBasicInfoAttributeAccess(driven_ports::CoverRepository& coverRepository);
    CHIP_ERROR Read(const chip::app::ConcreteReadAttributePath& path, chip::app::AttributeValueEncoder& encoder) override;

    // nothing to write
    CHIP_ERROR Write(const chip::app::ConcreteDataAttributePath& path, chip::app::AttributeValueDecoder& decoder) override { return CHIP_ERROR_INVALID_ARGUMENT; }

private:
    driven_ports::CoverRepository& mCoverRepository;
};

}
