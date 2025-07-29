#include "Statement.h"
#include <utility>

namespace mmbridge::common::persistence::sqlite {

Statement::Statement(sqlite3_stmt* stmt)
    : mStmt(stmt)
{
}

Statement::Statement(Statement&& other)
    : mStmt(std::exchange(other.mStmt, nullptr))
{
}

Statement::~Statement()
{
    if (mStmt) {
        sqlite3_finalize(mStmt);
    }
}

void Statement::bind(const int param, const std::string& value)
{
    sqlite3_bind_text(mStmt, param, value.c_str(), static_cast<int>(value.length()), SQLITE_TRANSIENT);
}

void Statement::bind(const int param, const char* value)
{
    sqlite3_bind_text(mStmt, param, value, -1, SQLITE_TRANSIENT);
}

void Statement::bind(const int param, const void* buf, int size)
{
    sqlite3_bind_blob(mStmt, param, buf, size, SQLITE_STATIC);
}

void Statement::bind(const int param, bool value)
{
    sqlite3_bind_int(mStmt, param, static_cast<int>(value));
}

void Statement::bind(const int param, const int32_t value)
{
    sqlite3_bind_int(mStmt, param, value);
}

void Statement::bind(const int param, const uint32_t value)
{
    sqlite3_bind_int64(mStmt, param, value);
}

void Statement::bind(const int param, const int64_t value)
{
    sqlite3_bind_int64(mStmt, param, value);
}

std::string Statement::columnAsString(const int index) const
{
    return reinterpret_cast<const char*>(sqlite3_column_text(mStmt, index));
}

const void* Statement::columnAsBlob(const int index) const
{
    return sqlite3_column_blob(mStmt, index);
}

bool Statement::columnAsBool(const int index) const
{
    return static_cast<bool>(sqlite3_column_int(mStmt, index));
}

int8_t Statement::columnAsInt8(const int index) const
{
    return sqlite3_column_int(mStmt, index);
}

int16_t Statement::columnAsInt16(const int index) const
{
    return sqlite3_column_int(mStmt, index);
}

int32_t Statement::columnAsInt32(const int index) const
{
    return sqlite3_column_int(mStmt, index);
}

int64_t Statement::columnAsInt64(const int index) const
{
    return sqlite3_column_int64(mStmt, index);
}

uint8_t Statement::columnAsUint8(const int index) const
{
    return static_cast<uint8_t>(sqlite3_column_int(mStmt, index));
}

uint16_t Statement::columnAsUint16(const int index) const
{
    return static_cast<uint16_t>(sqlite3_column_int(mStmt, index));
}

uint32_t Statement::columnAsUint32(const int index) const
{
    return static_cast<uint32_t>(sqlite3_column_int64(mStmt, index));
}

bool Statement::isColumnNull(const int index) const
{
    return SQLITE_NULL == sqlite3_column_type(mStmt, index);
}

int Statement::columnSize(const int index) const
{
    return sqlite3_column_bytes(mStmt, index);
}

int Statement::exec()
{
    sqlite3_step(mStmt);

    return sqlite3_changes(sqlite3_db_handle(mStmt));
}

bool Statement::fetch()
{
    return SQLITE_ROW == sqlite3_step(mStmt);
}

}
