#pragma once

#include "Statement.h"

#include <optional>
#include <sqlite3.h>
#include <string>

namespace mmbridge::common::persistence::sqlite {

class Connection final {
public:
    Connection(Connection&& other);
    ~Connection();

    static std::optional<Connection> open(const std::string& filename, int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);
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
