#pragma once

#include "application/driven_ports/CoverRepository.h"
#include "application/model/Flags.h"
#include "application/model/window_covering/CoverMotion.h"
#include "application/model/window_covering/CoverOperationalStatus.h"

#include <app-common/zap-generated/cluster-enums.h>
#include <app/AttributeAccessInterface.h>
#include <app/clusters/window-covering-server/window-covering-server.h>

namespace mobmatter::driving_adapters::matter::cover_cluster {

namespace model = mobmatter::application::model;
namespace driven_ports = mobmatter::application::driven_ports;

class CoverAttributeAccess final : public chip::app::AttributeAccessInterface {
public:
    explicit CoverAttributeAccess(driven_ports::CoverRepository& coverRepository);
    CHIP_ERROR Read(const chip::app::ConcreteReadAttributePath& path, chip::app::AttributeValueEncoder& encoder) override;

    // nothing to write
    CHIP_ERROR Write(const chip::app::ConcreteDataAttributePath& path, chip::app::AttributeValueDecoder& decoder) override { return CHIP_ERROR_INVALID_ARGUMENT; }

private:
    driven_ports::CoverRepository& mCoverRepository;

    chip::app::Clusters::WindowCovering::Type ConvertToType(const model::window_covering::CoverEndProductType coverEndProductType);
    chip::app::Clusters::WindowCovering::EndProductType ConvertToEndProductType(const model::window_covering::CoverEndProductType coverEndProductType);
    chip::BitMask<chip::app::Clusters::WindowCovering::ConfigStatus> ConvertToConfigStatus(const model::Flags<model::window_covering::CoverFeature>& featureFlags);
    chip::app::Clusters::WindowCovering::OperationalState ConvertToOperationalState(model::window_covering::CoverMotion motion);
    chip::BitMask<chip::app::Clusters::WindowCovering::OperationalStatus> ConvertToOperationalStatus(const model::window_covering::CoverOperationalStatus& operationalStatus);
    chip::BitMask<chip::app::Clusters::WindowCovering::Feature> ConvertToFeatureMap(const model::Flags<model::window_covering::CoverFeature>& featureFlags);
};

}
