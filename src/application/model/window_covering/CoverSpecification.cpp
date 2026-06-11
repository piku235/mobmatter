#include "CoverSpecification.h"

namespace mobmatter::application::model::window_covering {

std::optional<CoverSpecification> CoverSpecification::findFor(MobilusDeviceType mobilusDeviceType)
{
    switch (mobilusDeviceType) {
    case MobilusDeviceType::Senso:
        return Senso();
    case MobilusDeviceType::SensoZ:
        return SensoZ();
    case MobilusDeviceType::Cosmo:
        return Cosmo();
    case MobilusDeviceType::Cmr:
        return Cmr();
    default:
        return std::nullopt;
    }
}

CoverSpecification CoverSpecification::Senso()
{
    return {
        "Senso",
        MobilusDeviceType::Senso,
        CoverEndProductType::RollerShutter,
        {
            CoverFeature::Lift,
            CoverFeature::PositionAwareLift,
        }
    };
}

CoverSpecification CoverSpecification::SensoZ()
{
    return {
        "SensoZ",
        MobilusDeviceType::SensoZ,
        CoverEndProductType::VenetianBlind,
        {
            CoverFeature::Lift,
            CoverFeature::Tilt,
            CoverFeature::PositionAwareLift,
            CoverFeature::PositionAwareTilt,
        }
    };
}

CoverSpecification CoverSpecification::Cosmo()
{
    return {
        "Cosmo",
        MobilusDeviceType::Cosmo,
        CoverEndProductType::RollerShutter,
        {
            CoverFeature::Lift,
            CoverFeature::EdgePositionAwareLift,
        }
    };
}

CoverSpecification CoverSpecification::Cmr()
{
    return {
        "C-MR",
        MobilusDeviceType::Cmr,
        CoverEndProductType::RollerShutter,
        {
            CoverFeature::Lift,
            CoverFeature::EdgePositionAwareLift,
        }
    };
}

CoverSpecification::CoverSpecification(std::string model, MobilusDeviceType mobilusDeviceType, CoverEndProductType endProductType, Flags<CoverFeature> featureFlags)
    : mModel(std::move(model))
    , mMobilusDeviceType(mobilusDeviceType)
    , mEndProductType(endProductType)
    , mFeatureFlags(std::move(featureFlags))
{
}

bool CoverSpecification::operator==(const CoverSpecification& other) const
{
    return mModel == other.mModel
        && mMobilusDeviceType == other.mMobilusDeviceType
        && mEndProductType == other.mEndProductType
        && mFeatureFlags == other.mFeatureFlags;
}

}
