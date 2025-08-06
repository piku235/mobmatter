#pragma once

#include <string>

namespace mmbridge::common::domain {

template <typename TCode>
class DomainError final {
public:
    DomainError(TCode code, std::string message)
        : mCode(code)
        , mMessage(std::move(message))
    {
    }
    
    TCode code() const { return mCode; }
    const std::string& message() const { return mMessage; }

private:
    TCode mCode;
    std::string mMessage;
};

}
