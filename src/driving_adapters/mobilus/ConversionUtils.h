#pragma once

#include "application/model/window_covering/Position.h"
#include <string>

namespace mobmatter::driving_adapters::mobilus {

namespace ConversionUtils {
    std::optional<mobmatter::application::model::window_covering::Position> convertLiftPosition(const std::string& value);
}

}
