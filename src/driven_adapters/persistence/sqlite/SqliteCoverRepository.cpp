#include "SqliteCoverRepository.h"
#include "application/model/MobilusDeviceType.h"
#include "application/model/UniqueId.h"
#include "application/model/window_covering/CoverMotion.h"
#include "application/model/window_covering/CoverSpecification.h"
#include "application/model/window_covering/Position.h"
#include "application/model/window_covering/PositionState.h"
#include "application/model/window_covering/PositionStatus.h"

#define COLUMNS "endpoint_id, mobilus_device_id, unique_id, reachable, name, lift_status, lift_motion, lift_target_position, lift_current_position, spec_mobilus_device_type"

using namespace mobmatter::application::model;
using namespace mobmatter::application::model::window_covering;
namespace sqlite = mobmatter::common::persistence::sqlite;

namespace mobmatter::driven_adapters::persistence::sqlite {

SqliteCoverRepository::SqliteCoverRepository(sqlite::Connection& conn, logging::Logger& logger)
    : mConn(conn)
    , mLogger(logger)
{
}

void SqliteCoverRepository::save(const Cover& cover)
{
    auto stmt = mConn.prepare("INSERT OR REPLACE INTO cover (" COLUMNS ") VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");

    stmt->bind(1, cover.endpointId());
    stmt->bind(2, cover.mobilusDeviceId());
    stmt->bind(3, cover.uniqueId().value());
    stmt->bind(4, cover.isReachable());
    stmt->bind(5, cover.name());
    stmt->bind(6, static_cast<uint8_t>(cover.liftState().status()));
    stmt->bind(7, static_cast<uint8_t>(cover.liftState().motion()));
    stmt->bind(8, cover.liftState().targetPosition() ? std::optional(cover.liftState().targetPosition()->closedPercent().value()) : std::nullopt);
    stmt->bind(9, cover.liftState().currentPosition() ? std::optional(cover.liftState().currentPosition()->closedPercent().value()) : std::nullopt);
    stmt->bind(10, static_cast<uint8_t>(cover.specification().mobilusDeviceType()));

    if (auto r = stmt->exec(); !r) {
        mLogger.error("Could not save cover: %s", r.error().message().c_str());
    }
}

void SqliteCoverRepository::remove(const Cover& cover)
{
    auto stmt = mConn.prepare("DELETE FROM cover WHERE endpoint_id = ?");
    stmt->bind(1, cover.endpointId());

    if (auto r = stmt->exec(); !r) {
        mLogger.error("Could not remove cover: %s", r.error().message().c_str());
    }
}

std::optional<Cover> SqliteCoverRepository::findOfMobilusDeviceId(MobilusDeviceId deviceId) const
{
    auto stmt = mConn.prepare("SELECT " COLUMNS " FROM cover WHERE mobilus_device_id = ?");
    stmt->bind(1, deviceId);

    auto r = stmt->fetch();

    if (!r) {
        mLogger.error("Couldnt fetch cover of mobilus device id: %s", r.error().message().c_str());
        return std::nullopt;
    }

    if (!r.value()) {
        return std::nullopt;
    }

    return mapRowTo(*stmt);
}

std::optional<Cover> SqliteCoverRepository::find(EndpointId endpointId) const
{
    auto stmt = mConn.prepare("SELECT " COLUMNS " FROM cover WHERE endpoint_id = ?");
    stmt->bind(1, endpointId);

    auto r = stmt->fetch();

    if (!r) {
        mLogger.error("Couldnt fetch cover: %s", r.error().message().c_str());
        return std::nullopt;
    }

    if (!r.value()) {
        return std::nullopt;
    }

    return mapRowTo(*stmt);
}

std::vector<Cover> SqliteCoverRepository::all() const
{
    auto stmt = mConn.prepare("SELECT " COLUMNS " FROM cover");
    auto r = stmt->fetch();

    if (!r) {
        mLogger.error("Couldnt fetch all covers: %s", r.error().message().c_str());
        return {};
    }

    std::vector<Cover> covers;

    while (*r) {
        covers.push_back(mapRowTo(*stmt));
        r = stmt->fetch();
    }

    return covers;
}

Cover SqliteCoverRepository::mapRowTo(sqlite::Statement& stmt) const
{
    // clang-format off
    return Cover::restoreFrom(
        stmt.columnAsUint16(0),
        stmt.columnAsInt64(1),
        UniqueId::of(stmt.columnAsString(2)),
        stmt.columnAsBool(3),
        stmt.columnAsString(4),
        PositionState::restore(
            static_cast<PositionStatus>(stmt.columnAsUint8(5)),
            static_cast<CoverMotion>(stmt.columnAsUint8(6)),
            stmt.isColumnNull(7) ? std::nullopt : std::optional(Position::closed(Percent::from(stmt.columnAsUint8(7)).value())),
            stmt.isColumnNull(8) ? std::nullopt : std::optional(Position::closed(Percent::from(stmt.columnAsUint8(8)).value()))
        ),
        CoverSpecification::findFor(static_cast<MobilusDeviceType>(stmt.columnAsUint8(9))).value()
    );
    // clang-format on
}

}
