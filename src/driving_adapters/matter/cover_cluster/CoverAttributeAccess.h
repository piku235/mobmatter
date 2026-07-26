#pragma once

#include "application/driven_ports/CoverRepository.h"
#include "application/model/Flags.h"
#include "application/model/window_covering/CoverMotion.h"

#include <app/AttributeAccessInterface.h>
#include <app/clusters/window-covering-server/window-covering-server.h>

namespace mobmatter::driving_adapters::matter::cover_cluster {

namespace model = application::model;
namespace driven_ports = application::driven_ports;

class CoverAttributeAccess final : public chip::app::AttributeAccessInterface {
public:
    explicit CoverAttributeAccess(driven_ports::CoverRepository& coverRepository);

    CHIP_ERROR Read(const chip::app::ConcreteReadAttributePath& path, chip::app::AttributeValueEncoder& encoder) override;
    // nothing to write
    CHIP_ERROR Write(const chip::app::ConcreteDataAttributePath& path, chip::app::AttributeValueDecoder& decoder) override { return CHIP_ERROR_INVALID_ARGUMENT; }

private:
    driven_ports::CoverRepository& mCoverRepository;

    static chip::app::Clusters::WindowCovering::Type ConvertToType(model::window_covering::CoverEndProductType coverEndProductType);
    static chip::app::Clusters::WindowCovering::EndProductType ConvertToEndProductType(model::window_covering::CoverEndProductType coverEndProductType);
    static chip::BitMask<chip::app::Clusters::WindowCovering::ConfigStatus> ConvertToConfigStatus(const model::Flags<model::window_covering::CoverFeature>& featureFlags);
    static chip::app::Clusters::WindowCovering::OperationalState ConvertToOperationalState(model::window_covering::CoverMotion motion);
    static chip::BitMask<chip::app::Clusters::WindowCovering::OperationalStatus> ConvertToOperationalStatus(model::window_covering::CoverMotion liftMotion, model::window_covering::CoverMotion tiltMotion);
    static chip::BitMask<chip::app::Clusters::WindowCovering::Feature> ConvertToFeatureMap(const model::Flags<model::window_covering::CoverFeature>& featureFlags);
};

}
