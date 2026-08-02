#pragma once

#include "application/driven_ports/SwitchRepository.h"

#include <app/AttributeAccessInterface.h>

namespace mobmatter::driving_adapters::matter::switch_cluster {

namespace model = application::model;
namespace driven_ports = application::driven_ports;

class SwitchAttributeAccess final : public chip::app::AttributeAccessInterface {
public:
    explicit SwitchAttributeAccess(driven_ports::SwitchRepository& switchRepository);

    CHIP_ERROR Read(const chip::app::ConcreteReadAttributePath& path, chip::app::AttributeValueEncoder& encoder) override;
    // nothing to write
    CHIP_ERROR Write(const chip::app::ConcreteDataAttributePath& path, chip::app::AttributeValueDecoder& decoder) override { return CHIP_ERROR_INVALID_ARGUMENT; }

private:
    driven_ports::SwitchRepository& mSwitchRepository;
};

}
