#pragma once

#include "Result.h"
#include "Statement.h"

#include <sqlite3.h>

#include <string>

namespace mobmatter::common::persistence::sqlite {

class Connection final {
public:
    static Result<Connection> open(const std::string& filename, int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);
    static Result<Connection> inMemory();

    Connection(Connection&& other);
    Connection& operator=(Connection&& other) = delete;

    Connection(const Connection& other) = delete;
    Connection& operator=(const Connection& other) = delete;

    ~Connection();

    Result<Statement> prepare(const std::string& sql);
    Result<int> exec(const std::string& sql);
    void close();

private:
    sqlite3* mDb;

    explicit Connection(sqlite3* db);
};

}
