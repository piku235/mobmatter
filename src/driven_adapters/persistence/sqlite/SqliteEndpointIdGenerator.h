#pragma once

#include "application/driven_ports/EndpointIdGenerator.h"
#include "common/persistence/sqlite/Connection.h"
#include "common/logging/Logger.h"

namespace mobmatter::driven_adapters::persistence::sqlite {

namespace model = mobmatter::application::model;
namespace sqlite = mobmatter::common::persistence::sqlite;
namespace logging = mobmatter::common::logging;

class SqliteEndpointIdGenerator final : public mobmatter::application::driven_ports::EndpointIdGenerator {
public:
    SqliteEndpointIdGenerator(model::EndpointId initialEndpointId, sqlite::Connection& conn, logging::Logger& logger);
    std::optional<model::EndpointId> next() override;

private:
    model::EndpointId mInitialEndpointId;
    sqlite::Connection& mConn;
    logging::Logger& mLogger;
};

}
