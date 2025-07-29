#pragma once

#include <algorithm>
#include <cstdint>
#include <optional>

static constexpr uint8_t kPercentMax = 100;
static constexpr uint16_t kPercent100thsMax = 10000;

namespace mmbridge::application::model {

class Percent final {
public:
    static constexpr Percent min() { return 0; }
    static constexpr Percent max() { return kPercent100thsMax; }
    static std::optional<Percent> from(uint8_t value) { return value <= kPercentMax ? std::optional(Percent(value * 100)) : std::nullopt; }
    static std::optional<Percent> from100ths(uint16_t value100ths) { return value100ths <= kPercent100thsMax ? std::optional(Percent(value100ths)) : std::nullopt; }

    constexpr uint8_t value() const { return static_cast<uint8_t>(mValue / 100); }
    constexpr uint16_t value100ths() const { return mValue; }

    constexpr Percent operator+(const Percent& other) const { return std::min<uint16_t>(mValue + other.mValue, kPercent100thsMax); }
    constexpr Percent operator-(const Percent& other) const { return mValue < other.mValue ? 0 : mValue - other.mValue; }

    constexpr bool operator<(const Percent& other) const { return mValue < other.mValue; }
    constexpr bool operator>(const Percent& other) const { return mValue > other.mValue; }
    constexpr bool operator==(const Percent& other) const { return mValue == other.mValue; }
    constexpr bool operator!=(const Percent& other) const { return mValue != other.mValue; }
    constexpr bool operator>=(const Percent& other) const { return mValue >= other.mValue; }
    constexpr bool operator<=(const Percent& other) const { return mValue <= other.mValue; }

private:
    /* const */ uint16_t mValue;

    constexpr Percent(uint16_t value)
        : mValue(value)
    {
    }
};

}
