#pragma once

#include <cstdint>
#include <string>

namespace mmbridge::application::model {

static constexpr uint8_t kLength = 32;

class UniqueId final {
public:
    static UniqueId random();
    static UniqueId of(std::string value) { return value; }
    static constexpr uint8_t length() { return kLength; }

    bool operator==(const UniqueId& other) const { return mValue == other.mValue; }
    const std::string& value() const { return mValue; }

private:
    /* const */ std::string mValue;

    UniqueId(std::string value);
};

}
