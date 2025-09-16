#include "SqliteEndpointIdGenerator.h"

static constexpr char kSequenceName[] = "endpoint_id";

namespace sqlite = mobmatter::common::persistence::sqlite;
using mobmatter::application::model::EndpointId;

namespace mobmatter::driven_adapters::persistence::sqlite {

SqliteEndpointIdGenerator::SqliteEndpointIdGenerator(EndpointId initialEndpointId, sqlite::Connection& conn)
    : mInitialEndpointId(initialEndpointId)
    , mConn(conn)
{
}

EndpointId SqliteEndpointIdGenerator::next()
{
    auto updateStmt = mConn.prepare("UPDATE sequence SET value = value + 1 WHERE name = ? RETURNING value");
    updateStmt.bind(1, kSequenceName);

    if (!updateStmt.fetch()) {
        auto insertStmt = mConn.prepare("INSERT INTO sequence (name, value) VALUES (?, ?)");

        insertStmt.bind(1, kSequenceName);
        insertStmt.bind(2, mInitialEndpointId);
        insertStmt.exec();

        return mInitialEndpointId;
    }

    return updateStmt.columnAsUint16(0);
}

}
