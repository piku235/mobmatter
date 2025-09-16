#pragma once

#include "Statement.h"

#include <optional>
#include <sqlite3.h>
#include <string>

namespace mobmatter::common::persistence::sqlite {

class Connection final {
public:
    static std::optional<Connection> open(const std::string& filename, int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);
    static std::optional<Connection> inMemory();

    Connection(Connection&& other);
    ~Connection();

    Statement prepare(const std::string& sql);
    int exec(const std::string& sql);
    void close();

    Connection(const Connection& other) = delete;
    Connection& operator=(const Connection& other) = delete;
    Connection& operator=(Connection&& other) = delete;

private:
    sqlite3* mDb;

    explicit Connection(sqlite3* db);
};

}
