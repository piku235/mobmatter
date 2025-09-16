#pragma once

#include "application/model/Percent.h"

namespace mobmatter::application::model::window_covering {

/**
 * Chip uses the closed interpretation where closed = 100, open = 0
 * Mobilus uses the opened interpretation where closed = 0, open = 100
 *
 * Internally the closed interpretation is used
 */
class Position final {
public:
    static constexpr Position fullyOpen() { return Percent::min(); }
    static constexpr Position fullyClosed() { return Percent::max(); }
    static Position open(Percent percent) { return Percent::max() - percent; }
    static Position closed(Percent percent) { return percent; }

    constexpr Percent openPercent() const { return Percent::max() - mClosedPercent; }
    constexpr Percent closedPercent() const { return mClosedPercent; }
    constexpr bool isFullyOpen() const { return *this == fullyOpen(); }
    constexpr bool isFullyClosed() const { return *this == fullyClosed(); }
    constexpr bool isOpen() const { return *this != fullyClosed(); }

    constexpr bool operator==(const Position& other) const { return mClosedPercent == other.mClosedPercent; }
    constexpr bool operator!=(const Position& other) const { return mClosedPercent != other.mClosedPercent; }

private:
    /* const */ Percent mClosedPercent;

    constexpr Position(Percent closedPercent)
        : mClosedPercent(std::move(closedPercent))
    {
    }
};

}
