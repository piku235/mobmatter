#pragma once

#include "Connection.h"
#include "common/logging/Logger.h"

#include <cstdint>
#include <tl/expected.hpp>

namespace mobmatter::common::persistence::sqlite {

class Migrator final {
public:
    Migrator(Connection& connection, logging::Logger& logger);
    bool migrate(const char* migrations[], uint32_t count);

private:
    Connection& mConnection;
    logging::Logger& mLogger;

    tl::expected<uint32_t, std::string> queryCurrentVersion() const;
};

}
