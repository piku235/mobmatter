#pragma once

namespace mobmatter::application::model::window_covering {

enum class [[nodiscard]] PositionStatus {
    Unavailable,
    Idle,
    Moving,
    Stopping,
};

}
