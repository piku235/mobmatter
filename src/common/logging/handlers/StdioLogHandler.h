#pragma once

#include "common/logging/LogHandler.h"
#include "common/logging/LogLevel.h"

#include <cstdio>

namespace mobmatter::common::logging::handlers {

class StdioLogHandler final : public LogHandler {
public:
    void log(LogLevel level, const char* message) override;

private:
    FILE* logStream(LogLevel level);
    const char* levelName(LogLevel level);
};

}
