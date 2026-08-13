#include "SqliteSwitchRepository.h"

#include <cstdint>

#define COLUMNS "endpoint_id, mobilus_device_id, name, state"

using namespace mobmatter::application::model;
namespace sqlite = mobmatter::common::persistence::sqlite;

namespace mobmatter::driven_adapters::persistence::sqlite {

SqliteSwitchRepository::SqliteSwitchRepository(sqlite::Connection& conn, logging::Logger& logger)
    : mConn(conn)
    , mLogger(logger)
{
}

void SqliteSwitchRepository::save(const Switch& switch_)
{
    auto stmt = mConn.prepare("INSERT OR REPLACE INTO switch (" COLUMNS ") VALUES (?, ?, ?, ?)");

    stmt->bind(1, switch_.endpointId());
    stmt->bind(2, switch_.mobilusDeviceId());
    stmt->bind(3, switch_.name());
    stmt->bind(4, static_cast<uint8_t>(switch_.state()));

    if (auto r = stmt->exec(); !r) {
        mLogger.error("Could not save switch: %s", r.error().message().c_str());
    }
}

void SqliteSwitchRepository::remove(const Switch& switch_)
{
    auto stmt = mConn.prepare("DELETE FROM switch WHERE endpoint_id = ?");
    stmt->bind(1, switch_.endpointId());

    if (auto r = stmt->exec(); !r) {
        mLogger.error("Could not remove switch: %s", r.error().message().c_str());
    }
}

std::optional<Switch> SqliteSwitchRepository::findOfMobilusDeviceId(MobilusDeviceId deviceId) const
{
    auto stmt = mConn.prepare("SELECT " COLUMNS " FROM switch WHERE mobilus_device_id = ?");
    stmt->bind(1, deviceId);

    auto r = stmt->fetch();

    if (!r) {
        mLogger.error("Couldnt fetch switch of mobilus device id: %s", r.error().message().c_str());
        return std::nullopt;
    }

    if (!r.value()) {
        return std::nullopt;
    }

    return mapRowTo(*stmt);
}

std::optional<Switch> SqliteSwitchRepository::find(EndpointId endpointId) const
{
    auto stmt = mConn.prepare("SELECT " COLUMNS " FROM switch_ WHERE endpoint_id = ?");
    stmt->bind(1, endpointId);

    auto r = stmt->fetch();

    if (!r) {
        mLogger.error("Couldnt fetch switch: %s", r.error().message().c_str());
        return std::nullopt;
    }

    if (!r.value()) {
        return std::nullopt;
    }

    return mapRowTo(*stmt);
}

std::vector<Switch> SqliteSwitchRepository::all() const
{
    auto stmt = mConn.prepare("SELECT " COLUMNS " FROM switch");
    auto r = stmt->fetch();

    if (!r) {
        mLogger.error("Couldnt fetch all covers: %s", r.error().message().c_str());
        return { };
    }

    std::vector<Switch> covers;

    while (*r) {
        covers.push_back(mapRowTo(*stmt));
        r = stmt->fetch();
    }

    return covers;
}

Switch SqliteSwitchRepository::mapRowTo(sqlite::Statement& stmt)
{
    // clang-format off
    return Switch::restoreFrom(
        stmt.columnAsUint16(0),
        stmt.columnAsInt64(1),
        stmt.columnAsString(2),
        static_cast<Switch::State>(stmt.columnAsUint8(3))
    );
    // clang-format on
}

}
