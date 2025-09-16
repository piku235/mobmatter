#pragma once

#include "common/logging/LogHandler.h"

namespace mobmatter::common::logging::handlers {

class NoopLogHandler final : public LogHandler {
public:
    void log(LogLevel level, const char* message) override { }
};

}
