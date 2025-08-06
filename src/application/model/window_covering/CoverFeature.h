#pragma once

namespace mmbridge::application::model::window_covering {

enum class CoverFeature {
    Lift,
    PositionAwareLift,
    EdgePositionAwareLift, // non existing in matter
    // todo: tilt in later development
};

}
