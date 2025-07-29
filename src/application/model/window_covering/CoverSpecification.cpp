#include "CoverSpecification.h"

namespace mmbridge::application::model::window_covering {

std::optional<CoverSpecification> CoverSpecification::findFor(MobilusDeviceType mobilusDeviceType)
{
    switch (mobilusDeviceType) {
    case MobilusDeviceType::Senso:
        return CoverSpecification::Senso();
    case MobilusDeviceType::Cosmo:
        return CoverSpecification::Cosmo();
    case MobilusDeviceType::Cmr:
        return CoverSpecification::Cmr();
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

}
