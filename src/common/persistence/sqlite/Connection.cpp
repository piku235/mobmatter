#include "Connection.h"
#include <utility>

namespace mmbridge::common::persistence::sqlite {

std::optional<Connection> Connection::open(const std::string& filename, int flags)
{
    sqlite3* db;

    // todo: error
    if (SQLITE_OK != sqlite3_open_v2(filename.c_str(), &db, flags, nullptr)) {
        return std::nullopt;
    }

    return Connection(db);
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

Statement Connection::prepare(const std::string& sql)
{
    sqlite3_stmt* stmt;

    // todo: error
    sqlite3_prepare_v2(mDb, sql.c_str(), static_cast<int>(sql.length()), &stmt, nullptr);

    return Statement(stmt);
}

int Connection::exec(const std::string& sql)
{
    if (SQLITE_OK != sqlite3_exec(mDb, sql.c_str(), nullptr, nullptr, nullptr)) {
        return -1;
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
