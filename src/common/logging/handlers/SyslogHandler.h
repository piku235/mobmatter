#pragma once

#include "common/logging/LogHandler.h"
#include <cstdint>

namespace mmbridge::common::logging::handlers {

class SyslogHandler final : public LogHandler {
public:
    SyslogHandler() = default;
    explicit SyslogHandler(const char* ident);

    void log(LogLevel level, const char* message) override;
    uint8_t priority(LogLevel level);
};

}
