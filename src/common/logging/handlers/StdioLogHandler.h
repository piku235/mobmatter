#pragma once

#include "common/logging/LogHandler.h"
#include "common/logging/LogLevel.h"

namespace mmbridge::common::logging::handlers {

class StdioLogHandler final : public LogHandler {
public:
    void log(LogLevel level, const char* message) override;

private:
    const char* levelName(LogLevel level);
};

}
