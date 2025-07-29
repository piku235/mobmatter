#pragma once

#include "LogLevel.h"

namespace mmbridge::common::logging {

class LogHandler {
public:
    virtual ~LogHandler() = default;
    virtual void log(LogLevel level, const char* message) = 0;
};

}
