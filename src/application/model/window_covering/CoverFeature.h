#pragma once

#include <cstdint>

namespace mobmatter::application::model::window_covering {

enum class CoverFeature : uint8_t {
    Lift = 0x1,
    Tilt = 0x2,
    PositionAwareLift = 0x4,
    PositionAwareTilt = 0x8,
    EdgePositionAwareLift = 0x10, // non existing in matter
};

}
