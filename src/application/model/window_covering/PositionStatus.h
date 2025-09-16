#pragma once

namespace mobmatter::application::model::window_covering {

enum class PositionStatus {
    Unavailable,
    Idle,
    Requested,
    Moving,
    Stopping,
};

}
