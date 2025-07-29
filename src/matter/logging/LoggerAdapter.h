#pragma once

#include "common/logging/Logger.h"

namespace mmbridge::matter::logging {

namespace LoggerAdapter {
    using mmbridge::common::logging::Logger;

    void setLogger(Logger* logger);
    Logger* getLogger();
}

}
