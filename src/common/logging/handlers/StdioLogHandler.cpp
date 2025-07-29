#include "StdioLogHandler.h"

#include <cinttypes>
#include <cstdint>
#include <cstdio>
#include <sys/time.h>

namespace mmbridge::common::logging::handlers {

void StdioLogHandler::log(LogLevel level, const char* message)
{
    timeval tv;
    gettimeofday(&tv, nullptr);

    printf("[%" PRIu64 ".%06" PRIu64 "][%s] %s\n", static_cast<uint64_t>(tv.tv_sec), static_cast<uint64_t>(tv.tv_usec), levelName(level), message);
    fflush(stdout);
}

const char* StdioLogHandler::levelName(LogLevel level)
{
    switch (level) {
    case LogLevel::Critical:
        return "CRITICAL";
    case LogLevel::Error:
        return "ERROR";
    case LogLevel::Warning:
        return "WARNING";
    case LogLevel::Notice:
        return "NOTICE";
    case LogLevel::Info:
        return "INFO";
    case LogLevel::Debug:
        return "DEBUG";
    default:
        return "UNKNOWN";
    }
}

}
