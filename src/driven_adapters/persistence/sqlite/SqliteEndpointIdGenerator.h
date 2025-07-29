#pragma once

#include "application/driven_ports/EndpointIdGenerator.h"
#include "common/persistence/sqlite/Connection.h"

namespace mmbridge::driven_adapters::persistence::sqlite {

namespace model = mmbridge::application::model;
namespace sqlite = mmbridge::common::persistence::sqlite;

class SqliteEndpointIdGenerator final : public mmbridge::application::driven_ports::EndpointIdGenerator {
public:
    SqliteEndpointIdGenerator(model::EndpointId initialEndpointId, sqlite::Connection& conn);
    model::EndpointId next() override;

private:
    model::EndpointId mInitialEndpointId;
    sqlite::Connection& mConn;
};

}
