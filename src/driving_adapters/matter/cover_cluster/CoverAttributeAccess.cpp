#include "CoverAttributeAccess.h"

#include <app-common/zap-generated/ids/Attributes.h>
#include <app-common/zap-generated/ids/Clusters.h>

#include <cstdint>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::WindowCovering::Attributes;
using namespace mmbridge::application::model::window_covering;
using mmbridge::application::driven_ports::CoverRepository;
using mmbridge::application::model::Flags;

namespace {

constexpr uint8_t kWindowCoveringMode = 0u;
constexpr uint16_t kWindowCoveringClusterRevision = 5u;

}

namespace mmbridge::driving_adapters::matter::cover_cluster {

CoverAttributeAccess::CoverAttributeAccess(CoverRepository& coverRepository)
    : AttributeAccessInterface(Optional<EndpointId>::Missing(), WindowCovering::Id)
    , mCoverRepository(coverRepository)
{
}

CHIP_ERROR CoverAttributeAccess::Read(const ConcreteReadAttributePath& path, AttributeValueEncoder& encoder)
{
    auto cover = mCoverRepository.find(path.mEndpointId);

    if (!cover) {
        return CHIP_ERROR_NOT_FOUND;
    }

    switch (path.mAttributeId) {
    case Type::Id:
        return encoder.Encode(ConvertToType(cover->specification().endProductType()));
    case EndProductType::Id:
        return encoder.Encode(ConvertToEndProductType(cover->specification().endProductType()));
    case ConfigStatus::Id:
        return encoder.Encode(ConvertToConfigStatus(cover->specification().featureFlags()));
    case OperationalStatus::Id:
        return encoder.Encode(ConvertToOperationalStatus(cover->operationalStatus()));
    case Mode::Id:
        return encoder.Encode(kWindowCoveringMode);
    case TargetPositionLiftPercent100ths::Id: {
        auto targetPosition = cover->liftState().targetPosition();

        if (!targetPosition) {
            return CHIP_ERROR_UNINITIALIZED;
        }

        return encoder.Encode(targetPosition->closedPercent().value100ths());
    }
    case CurrentPositionLiftPercent100ths::Id: {
        auto currentPosition = cover->liftState().currentPosition();

        if (!currentPosition) {
            return CHIP_ERROR_UNINITIALIZED;
        }

        return encoder.Encode(currentPosition->closedPercent().value100ths());
    }
    case FeatureMap::Id:
        return encoder.Encode(ConvertToFeatureMap(cover->specification().featureFlags()));
    case ClusterRevision::Id:
        return encoder.Encode(kWindowCoveringClusterRevision);
    default:
        return CHIP_ERROR_INVALID_ARGUMENT;
    }
}

WindowCovering::Type CoverAttributeAccess::ConvertToType(const model::window_covering::CoverEndProductType coverEndProductType)
{
    switch (coverEndProductType) {
    case CoverEndProductType::RollerShutter:
        return WindowCovering::Type::kRollerShadeExterior;
    case CoverEndProductType::Unknown:
    default:
        return WindowCovering::Type::kUnknown;
    }
}

WindowCovering::EndProductType CoverAttributeAccess::ConvertToEndProductType(const model::window_covering::CoverEndProductType coverEndProductType)
{
    switch (coverEndProductType) {
    case CoverEndProductType::RollerShutter:
        return WindowCovering::EndProductType::kRollerShutter;
    case CoverEndProductType::Unknown:
    default:
        return WindowCovering::EndProductType::kUnknown;
    }
}

BitMask<WindowCovering::ConfigStatus> CoverAttributeAccess::ConvertToConfigStatus(const Flags<CoverFeature>& featureFlags)
{
    BitMask<WindowCovering::ConfigStatus> bitMask;

    bitMask.Set(WindowCovering::ConfigStatus::kOperational, 1u);
    bitMask.Set(WindowCovering::ConfigStatus::kLiftPositionAware, featureFlags.has(CoverFeature::PositionAwareLift));

    return bitMask;
}

WindowCovering::OperationalState CoverAttributeAccess::ConvertToOperationalState(CoverMotion motion)
{
    switch (motion) {
    case CoverMotion::Opening:
        return WindowCovering::OperationalState::MovingUpOrOpen;
    case CoverMotion::Closing:
        return WindowCovering::OperationalState::MovingDownOrClose;
    case CoverMotion::NotMoving:
    default:
        return WindowCovering::OperationalState::Stall;
    }
}

BitMask<WindowCovering::OperationalStatus> CoverAttributeAccess::ConvertToOperationalStatus(const CoverOperationalStatus& operationalStatus)
{
    BitMask<WindowCovering::OperationalStatus> bitMask;

    bitMask.SetField(WindowCovering::OperationalStatus::kGlobal, static_cast<uint8_t>(ConvertToOperationalState(operationalStatus.global())));
    bitMask.SetField(WindowCovering::OperationalStatus::kLift, static_cast<uint8_t>(ConvertToOperationalState(operationalStatus.lift())));
    bitMask.SetField(WindowCovering::OperationalStatus::kTilt, static_cast<uint8_t>(ConvertToOperationalState(operationalStatus.tilt())));

    return bitMask;
}

BitMask<WindowCovering::Feature> CoverAttributeAccess::ConvertToFeatureMap(const Flags<CoverFeature>& featureFlags)
{
    BitMask<WindowCovering::Feature> bitMask;

    bitMask.Set(WindowCovering::Feature::kLift, featureFlags.has(CoverFeature::Lift));
    bitMask.Set(WindowCovering::Feature::kPositionAwareLift, featureFlags.has(CoverFeature::PositionAwareLift));

    return bitMask;
}

}
