#pragma once

#include <cstdint>
#include <initializer_list>
#include <type_traits>

namespace mmbridge::application::model {

template <typename T, typename = std::enable_if_t<std::is_enum<T>::value>>
class Flags final {
public:
    using TUnly = std::underlying_type_t<T>;

    Flags() = default;

    Flags(TUnly mask)
        : mMask(mask)
    {
    }

    Flags(std::initializer_list<T> list)
    {
        for (auto value : list) {
            set(value);
        }
    }

    void set(T value) { mMask |= static_cast<TUnly>(value); }
    void unset(T value) { mMask &= ~static_cast<TUnly>(value); }
    bool has(T value) const { return mMask & static_cast<TUnly>(value); }
    TUnly mask() const { return mMask; }

    bool operator==(const Flags& other) const { return mMask == other.mMask; }

private:
    TUnly mMask = 0;
};

}
