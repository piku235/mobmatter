#include "MobilusCoverPositionState.h"

#include <cstdint>
#include <cstdio>
#include <string_view>

using mobmatter::application::model::Percent;
using mobmatter::application::model::window_covering::Position;

namespace {

std::optional<Position> parseLiftPosition(std::string_view value)
{
    uint8_t parsedValue;
    int read = -1;

    if (1 == sscanf(value.data(), "%hhu%%%n", &parsedValue, &read) && read == value.size()) {
        if (auto percent = Percent::from(parsedValue)) {
            return Position::open(*percent);
        }

        return std::nullopt;
    }

    if ("UP" == value) {
        return Position::fullyOpen();
    }

    if ("DOWN" == value) {
        return Position::fullyClosed();
    }

    return std::nullopt;
}

std::optional<Position> parseTiltPosition(std::string_view value)
{
    uint8_t parsedValue;
    int read = -1;

    if (1 == sscanf(value.data(), "%hhu$%n", &parsedValue, &read) && read == value.size()) {
        if (auto percent = Percent::from(parsedValue)) {
            return Position::open(*percent);
        }

        return std::nullopt;
    }

    return std::nullopt;
}

}

namespace mobmatter::driving_adapters::mobilus::device_handlers {

MobilusCoverPositionState MobilusCoverPositionState::parse(const std::string& value)
{
    std::string_view sv(value);

    if (auto pos = sv.find(':'); pos != std::string::npos) {
        auto liftPosition = parseLiftPosition(sv.substr(0, pos));
        auto tiltPosition = parseTiltPosition(sv.substr(pos + 1));

        if (liftPosition && tiltPosition) {
            return { std::move(liftPosition), std::move(tiltPosition) };
        }

        return { std::nullopt, std::nullopt };
    }

    if (auto liftPosition = parseLiftPosition(sv); liftPosition) {
        return { std::move(liftPosition), std::nullopt };
    }

    if (auto tiltPosition = parseTiltPosition(sv); tiltPosition) {
        return { std::nullopt, std::move(tiltPosition) };
    }

    return { std::nullopt, std::nullopt };
}

}
