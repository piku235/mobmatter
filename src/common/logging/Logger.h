#pragma once

#include "LogHandler.h"
#include "LogLevel.h"
#include "handlers/NoopLogHandler.h"

#include <cstdarg>
#include <memory>

namespace mobmatter::common::logging {

class Logger final {
public:
    static Logger& noop();
    explicit Logger(std::unique_ptr<LogHandler> handler);

    void critical(const char* format, ...);
    void error(const char* format, ...);
    void warning(const char* format, ...);
    void notice(const char* format, ...);
    void info(const char* format, ...);
    void debug(const char* format, ...);
    void log(LogLevel level, const char* format, ...);

private:
    std::unique_ptr<LogHandler> mHandler;

    void log(LogLevel level, const char* format, va_list args);
};

}
