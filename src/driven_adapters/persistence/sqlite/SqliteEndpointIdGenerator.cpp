#include "SqliteEndpointIdGenerator.h"

static constexpr char kSequenceName[] = "endpoint_id";

namespace sqlite = mobmatter::common::persistence::sqlite;
using mobmatter::application::model::EndpointId;

namespace mobmatter::driven_adapters::persistence::sqlite {

SqliteEndpointIdGenerator::SqliteEndpointIdGenerator(EndpointId initialEndpointId, sqlite::Connection& conn, logging::Logger& logger)
    : mInitialEndpointId(initialEndpointId)
    , mConn(conn)
    , mLogger(logger)
{
}

std::optional<model::EndpointId> SqliteEndpointIdGenerator::next()
{
    auto updateStmt = mConn.prepare("UPDATE sequence SET value = value + 1 WHERE name = ? RETURNING value");
    updateStmt->bind(1, kSequenceName);

    auto updateRes = updateStmt->fetch();

    if (!updateRes) {
        mLogger.error("Sequent update failed: %s", updateRes.error().message().c_str());
        return std::nullopt;
    }

    if (!updateRes.value()) {
        auto insertStmt = mConn.prepare("INSERT INTO sequence (name, value) VALUES (?, ?)");

        insertStmt->bind(1, kSequenceName);
        insertStmt->bind(2, mInitialEndpointId);

        if (auto r = insertStmt->exec(); !r) {
            mLogger.error("Sequent insert failed: %s", r.error().message().c_str());
            return std::nullopt;
        }

        return mInitialEndpointId;
    }

    return updateStmt->columnAsUint16(0);
}

}
