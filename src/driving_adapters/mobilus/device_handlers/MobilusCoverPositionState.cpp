#include "MobilusCoverPositionState.h"

#include <charconv>
#include <cstdint>
#include <string_view>

using mobmatter::application::model::Percent;
using mobmatter::application::model::window_covering::Position;

static std::optional<Position> parsePosition(std::string_view value, char sign)
{
    if (!value.ends_with(sign)) {
        return std::nullopt;
    }

    uint8_t percentValue;
    auto end = value.data() + value.size() - 1;
    auto [ptr, ec] = std::from_chars(value.data(), end, percentValue);

    if (ec != std::errc { } || ptr != end) {
        return std::nullopt;
    }

    if (auto percent = Percent::from(percentValue)) {
        return Position::open(*percent);
    }

    return std::nullopt;
}

namespace {

std::optional<Position> parseLiftPosition(std::string_view value)
{
    return parsePosition(value, '%');
}

std::optional<Position> parseTiltPosition(std::string_view value)
{
    return parsePosition(value, '$');
}

}

namespace mobmatter::driving_adapters::mobilus::device_handlers {

MobilusCoverPositionState MobilusCoverPositionState::parse(const std::string& value)
{
    std::string_view sv(value);

    if (auto pos = sv.find(':'); pos != std::string::npos) {
        auto liftPosition = value.starts_with("DOWN")
            ? Position::fullyClosed()
            : parseLiftPosition(sv.substr(0, pos));
        auto tiltPosition = parseTiltPosition(sv.substr(pos + 1));

        if (liftPosition && tiltPosition) {
            return { liftPosition, tiltPosition };
        }

        return { std::nullopt, std::nullopt };
    }

    if (auto liftPosition = parseLiftPosition(sv); liftPosition) {
        return { liftPosition, std::nullopt };
    }

    if (auto tiltPosition = parseTiltPosition(sv); tiltPosition) {
        return { std::nullopt, tiltPosition };
    }

    return { std::nullopt, std::nullopt };
}

}
