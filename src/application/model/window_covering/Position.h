#pragma once

#include "application/model/Percent.h"

namespace mobmatter::application::model::window_covering {

/**
 * Chip uses the closed interpretation where closed = 100, open = 0
 * Mobilus uses the opened interpretation where closed = 0, open = 100
 *
 * Internally the closed interpretation is used
 */
class [[nodiscard]] Position final {
public:
    static constexpr Position fullyOpen() { return Percent::min(); }
    static constexpr Position fullyClosed() { return Percent::max(); }
    static Position open(Percent percent) { return Percent::max() - percent; }
    static Position closed(Percent percent) { return percent; }

    constexpr Percent openPercent() const { return Percent::max() - mClosedPercent; }
    constexpr Percent closedPercent() const { return mClosedPercent; }
    [[nodiscard]] constexpr bool isFullyOpen() const { return *this == fullyOpen(); }
    [[nodiscard]] constexpr bool isFullyClosed() const { return *this == fullyClosed(); }
    [[nodiscard]] constexpr bool isOpen() const { return *this != fullyClosed(); }

    constexpr bool operator==(const Position& other) const = default;
    constexpr bool operator!=(const Position& other) const = default;

private:
    /* const */ Percent mClosedPercent;

    constexpr Position(Percent closedPercent)
        : mClosedPercent(closedPercent)
    {
    }
};

}
