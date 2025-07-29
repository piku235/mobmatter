#pragma once

#include "application/model/window_covering/Position.h"
#include <string>

namespace mmbridge::driving_adapters::mobilus {

namespace ConversionUtils {
    std::optional<mmbridge::application::model::window_covering::Position> convertLiftPosition(const std::string& value);
}

}
