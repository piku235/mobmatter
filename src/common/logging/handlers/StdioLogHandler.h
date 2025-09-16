#pragma once

#include "common/logging/LogHandler.h"
#include "common/logging/LogLevel.h"

namespace mobmatter::common::logging::handlers {

class StdioLogHandler final : public LogHandler {
public:
    void log(LogLevel level, const char* message) override;

private:
    const char* levelName(LogLevel level);
};

}
