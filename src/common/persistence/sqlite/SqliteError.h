#pragma once

#include <sqlite3.h>

#include <string>

namespace mobmatter::common::persistence::sqlite {

class SqliteError final {
public:
    static SqliteError recent(sqlite3* conn)
    {
        return { sqlite3_errcode(conn), sqlite3_errmsg(conn) };
    }

    SqliteError(int code, std::string message)
        : mCode(code)
        , mMessage(message)
    {
    }

    int code() const { return mCode; }
    const std::string& message() const { return mMessage; }

private:
    int mCode;
    std::string mMessage;
};

}
