#include "Migrator.h"

namespace mobmatter::common::persistence::sqlite {

Migrator::Migrator(Connection& connection, logging::Logger& logger)
    : mConnection(connection)
    , mLogger(logger)
{
}

bool Migrator::migrate(const char* migrations[], uint32_t count)
{
    auto currentVersion = queryCurrentVersion();

    if (!currentVersion) {
        mLogger.error("Failed to query current migration version: %s", currentVersion.error().c_str());
        return false;
    }

    for (uint32_t i = 0; i < count; ++i) {
        uint32_t version = i + 1;

        if (version <= *currentVersion) {
            continue;
        }

        std::string sql;
        sql += "BEGIN;";
        sql += std::string(migrations[i]);
        sql += "PRAGMA user_version = " + std::to_string(version) + ";";
        sql += "COMMIT;";

        if (auto result = mConnection.exec(sql); !result) {
            mLogger.error("Failed to execute migration v%u: %s", version, result.error().message().c_str());
            return false;
        }

        mLogger.notice("Executed migration v%u", version);
    }

    return true;
}

tl::expected<uint32_t, std::string> Migrator::queryCurrentVersion() const
{
    auto stmt = mConnection.prepare("PRAGMA user_version");
    if (!stmt) {
        return tl::unexpected(stmt.error().message());
    }

    auto result = stmt->fetch();
    if (!result) {
        return tl::unexpected(result.error().message());
    }

    if (!result.value()) {
        return tl::unexpected("PRAGMA user_version no result");
    }

    return stmt->columnAsUint32(0);
}

}
