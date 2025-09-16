#pragma once

#include "LogLevel.h"

namespace mobmatter::common::logging {

class LogHandler {
public:
    virtual ~LogHandler() = default;
    virtual void log(LogLevel level, const char* message) = 0;
};

}
