#include "StdioLogHandler.h"

#include <cinttypes>
#include <cstdint>
#include <sys/time.h>

namespace mobmatter::common::logging::handlers {

void StdioLogHandler::log(LogLevel level, const char* message)
{
    timeval tv;
    gettimeofday(&tv, nullptr);
    FILE* stream = logStream(level);

    fprintf(stream, "[%" PRIu64 ".%06" PRIu64 "][%s] %s\n", static_cast<uint64_t>(tv.tv_sec), static_cast<uint64_t>(tv.tv_usec), levelName(level), message);
    fflush(stream);
}

FILE* StdioLogHandler::logStream(LogLevel level)
{
    switch (level) {
    case LogLevel::Critical:
    case LogLevel::Error:
        return stderr;
    default:
        return stdout;
    }
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
