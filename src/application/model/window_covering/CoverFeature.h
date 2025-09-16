#pragma once

#include <cstdint>

namespace mobmatter::application::model::window_covering {

enum class CoverFeature : uint8_t {
    Lift = 0x1,
    PositionAwareLift = 0x2,
    EdgePositionAwareLift = 0x4, // non existing in matter
    // todo: tilt in later development
};

}
