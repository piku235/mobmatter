#pragma once

#include "CoverMotion.h"

namespace mmbridge::application::model::window_covering {

struct CoverOperationalStatus final {
public:
    CoverOperationalStatus(CoverMotion lift, CoverMotion tilt)
        : mLift(lift)
        , mTilt(tilt)
    {
    }

    CoverMotion global() const { return CoverMotion::NotMoving != mLift ? mLift : mTilt; }
    CoverMotion lift() const { return mLift; }
    CoverMotion tilt() const { return mTilt; }

    bool operator==(const CoverOperationalStatus& other) const { return mLift == other.mLift && mTilt == other.mTilt; }

private:
    /* const */ CoverMotion mLift;
    /* const */ CoverMotion mTilt;
};

}
