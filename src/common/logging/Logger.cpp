#include "Logger.h"
#include <cstdio>

namespace mmbridge::common::logging {

Logger& Logger::noop()
{
    static Logger logger(std::make_unique<handlers::NoopLogHandler>());

    return logger;
}

Logger::Logger(std::unique_ptr<LogHandler> handler)
    : mHandler(std::move(handler))
{
}

void Logger::critical(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    log(LogLevel::Critical, format, args);
    va_end(args);
}

void Logger::error(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    log(LogLevel::Error, format, args);
    va_end(args);
}

void Logger::warning(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    log(LogLevel::Warning, format, args);
    va_end(args);
}

void Logger::notice(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    log(LogLevel::Notice, format, args);
    va_end(args);
}

void Logger::info(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    log(LogLevel::Info, format, args);
    va_end(args);
}

void Logger::debug(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    log(LogLevel::Debug, format, args);
    va_end(args);
}

void Logger::log(LogLevel level, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    log(level, format, args);
    va_end(args);
}

void Logger::log(LogLevel level, const char* format, va_list args)
{
    char buf[256];

    std::vsnprintf(buf, sizeof(buf), format, args);
    mHandler->log(level, buf);
}

}
