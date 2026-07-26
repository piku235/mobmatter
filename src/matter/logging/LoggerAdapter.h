#pragma once

#include "common/logging/Logger.h"

namespace mobmatter::matter::logging {

namespace LoggerAdapter {
    using common::logging::Logger;

    void setLogger(Logger* logger);
    Logger* getLogger();
}

}
