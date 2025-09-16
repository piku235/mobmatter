#include "ConversionUtils.h"

using mobmatter::application::model::Percent;
using mobmatter::application::model::window_covering::Position;

namespace mobmatter::driving_adapters::mobilus {

std::optional<Position> ConversionUtils::convertLiftPosition(const std::string& value)
{
    uint8_t parsedPercent;

    if ('%' == value.back() && 1 == sscanf(value.c_str(), "%hhu", &parsedPercent)) {
        auto percent = Percent::from(parsedPercent);

        if (percent) {
            return Position::open(*percent);
        }

        return std::nullopt;
    }

    if (!value.compare("UP")) {
        return Position::fullyOpen();
    }

    if (!value.compare("DOWN")) {
        return Position::fullyClosed();
    }

    return std::nullopt;
}

}
