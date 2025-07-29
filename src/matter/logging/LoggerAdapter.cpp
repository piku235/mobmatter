#include "LoggerAdapter.h"
#include "common/logging/LogLevel.h"

#include <lib/core/CHIPConfig.h>
#include <lib/support/logging/Constants.h>
#include <platform/logging/LogV.h>

#include <cstdio>

using namespace mmbridge::common::logging;
using namespace mmbridge::matter::logging;

namespace mmbridge::matter::logging {

static Logger* sLogger = nullptr;

void LoggerAdapter::setLogger(Logger* logger)
{
    sLogger = logger;
}

Logger* LoggerAdapter::getLogger()
{
    return sLogger;
}

}

static LogLevel logLevel(uint8_t category)
{
    using namespace chip::Logging;

    switch (category) {
    case kLogCategory_Error:
        return LogLevel::Error;
    case kLogCategory_Progress:
        return LogLevel::Info;
    default:
        return LogLevel::Debug;
    }
}

void chip::Logging::Platform::LogV(const char* module, uint8_t category, const char* msg, va_list v)
{
    auto logger = LoggerAdapter::getLogger();

    if (nullptr == logger) {
        return;
    }

    char formattedMsg[CHIP_CONFIG_LOG_MESSAGE_MAX_SIZE];
    std::vsnprintf(formattedMsg, sizeof(formattedMsg), msg, v);

    logger->log(logLevel(category), "%s: %s", module, formattedMsg);
}
