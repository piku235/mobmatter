#pragma once

#include "application/model/window_covering/Position.h"

#include <string>
#include <optional>

namespace mobmatter::driving_adapters::mobilus::device_handlers {

namespace model = mobmatter::application::model;

struct MobilusCoverPositionState final {
    std::optional<model::window_covering::Position> liftPosition;
    std::optional<model::window_covering::Position> tiltPosition;

    static MobilusCoverPositionState parse(const std::string& value);
    bool isValid() const { return liftPosition.has_value() || tiltPosition.has_value(); }
};

}
