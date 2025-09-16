#pragma once

#include "CoverEndProductType.h"
#include "CoverFeature.h"
#include "application/model/Flags.h"
#include "application/model/MobilusDeviceType.h"

#include <optional>
#include <string>

namespace mobmatter::application::model::window_covering {

class CoverSpecification final {
public:
    static std::optional<CoverSpecification> findFor(MobilusDeviceType mobilusDeviceType);
    static CoverSpecification Senso();
    static CoverSpecification Cosmo();
    static CoverSpecification Cmr();
    static CoverSpecification TestCzr(); // internal

    const std::string& model() const { return mModel; }
    MobilusDeviceType mobilusDeviceType() const { return mMobilusDeviceType; }
    CoverEndProductType endProductType() const { return mEndProductType; }
    Flags<CoverFeature> featureFlags() const { return mFeatureFlags; }

    bool operator==(const CoverSpecification& other) const;

private:
    /* const */ std::string mModel;
    /* const */ MobilusDeviceType mMobilusDeviceType;
    /* const */ CoverEndProductType mEndProductType;
    /* const */ Flags<CoverFeature> mFeatureFlags;

    CoverSpecification(std::string model, MobilusDeviceType mobilusDeviceType, CoverEndProductType endProductType, Flags<CoverFeature> featureFlags);
};

}
