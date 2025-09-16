#pragma once

#include <app/AttributeAccessInterface.h>

namespace mobmatter::driving_adapters::matter::cluster_stubs {

class IdentifyAttributeAccessStub final : public chip::app::AttributeAccessInterface {
public:
    IdentifyAttributeAccessStub();
    CHIP_ERROR Read(const chip::app::ConcreteReadAttributePath& path, chip::app::AttributeValueEncoder& encoder) override;

    // nothing to write
    CHIP_ERROR Write(const chip::app::ConcreteDataAttributePath& path, chip::app::AttributeValueDecoder& decoder) override { return CHIP_ERROR_INVALID_ARGUMENT; }
};

}
