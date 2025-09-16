#pragma once

#include "application/driven_ports/EndpointIdGenerator.h"
#include "common/persistence/sqlite/Connection.h"

namespace mobmatter::driven_adapters::persistence::sqlite {

namespace model = mobmatter::application::model;
namespace sqlite = mobmatter::common::persistence::sqlite;

class SqliteEndpointIdGenerator final : public mobmatter::application::driven_ports::EndpointIdGenerator {
public:
    SqliteEndpointIdGenerator(model::EndpointId initialEndpointId, sqlite::Connection& conn);
    model::EndpointId next() override;

private:
    model::EndpointId mInitialEndpointId;
    sqlite::Connection& mConn;
};

}
