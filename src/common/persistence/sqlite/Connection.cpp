#include "Connection.h"
#include "SqliteError.h"

#include <utility>

namespace mobmatter::common::persistence::sqlite {

Result<Connection> Connection::open(const std::string& filename, int flags)
{
    sqlite3* db;

    if (SQLITE_OK != sqlite3_open_v2(filename.c_str(), &db, flags, nullptr)) {
        return tl::unexpected(SqliteError::recent(db));
    }

    return Connection(db);
}

Result<Connection> Connection::inMemory()
{
    return open(":memory:");
}

Connection::Connection(sqlite3* db)
    : mDb(db)
{
}

Connection::Connection(Connection&& other)
    : mDb(std::exchange(other.mDb, nullptr))
{
}

Connection::~Connection()
{
    close();
}

Result<Statement> Connection::prepare(const std::string& sql)
{
    sqlite3_stmt* stmt;

    if (SQLITE_OK != sqlite3_prepare_v2(mDb, sql.c_str(), static_cast<int>(sql.length()), &stmt, nullptr)) {
        return tl::unexpected(SqliteError::recent(mDb));
    }

    return Statement(stmt);
}

Result<int> Connection::exec(const std::string& sql)
{
    if (SQLITE_OK != sqlite3_exec(mDb, sql.c_str(), nullptr, nullptr, nullptr)) {
        return tl::unexpected(SqliteError::recent(mDb));
    }

    return sqlite3_changes(mDb);
}

void Connection::close()
{
    if (mDb) {
        sqlite3_close(mDb);
    }
}

}
