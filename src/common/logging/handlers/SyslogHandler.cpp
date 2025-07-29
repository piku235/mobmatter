#include "SyslogHandler.h"
#include <syslog.h>

namespace mmbridge::common::logging::handlers {

SyslogHandler::SyslogHandler(const char* ident)
{
    openlog(ident, LOG_PID | LOG_CONS, LOG_DAEMON);
}

void SyslogHandler::log(LogLevel level, const char* message)
{
    syslog(priority(level), "%s", message);
}

uint8_t SyslogHandler::priority(LogLevel level)
{
    switch (level) {
    case LogLevel::Critical:
        return LOG_CRIT;
    case LogLevel::Error:
        return LOG_ERR;
    case LogLevel::Warning:
        return LOG_WARNING;
    case LogLevel::Notice:
        return LOG_NOTICE;
    case LogLevel::Info:
        return LOG_INFO;
    case LogLevel::Debug:
        return LOG_DEBUG;
    default:
        return LOG_EMERG;
    }
}

}
