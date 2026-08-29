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
    [[nodiscard]] static std::optional<CoverSpecification> findFor(MobilusDeviceType mobilusDeviceType);
    [[nodiscard]] static CoverSpecification Senso();
    [[nodiscard]] static CoverSpecification SensoZ();
    [[nodiscard]] static CoverSpecification Cosmo();
    [[nodiscard]] static CoverSpecification Cmr();

    [[nodiscard]] const std::string& model() const { return mModel; }
    [[nodiscard]] MobilusDeviceType mobilusDeviceType() const { return mMobilusDeviceType; }
    [[nodiscard]] CoverEndProductType endProductType() const { return mEndProductType; }
    [[nodiscard]] Flags<CoverFeature> featureFlags() const { return mFeatureFlags; }

    bool operator==(const CoverSpecification& other) const;

private:
    /* const */ std::string mModel;
    /* const */ MobilusDeviceType mMobilusDeviceType;
    /* const */ CoverEndProductType mEndProductType;
    /* const */ Flags<CoverFeature> mFeatureFlags;

    CoverSpecification(std::string model, MobilusDeviceType mobilusDeviceType, CoverEndProductType endProductType, Flags<CoverFeature> featureFlags);
};

}
