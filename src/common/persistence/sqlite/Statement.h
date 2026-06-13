#pragma once

#include "Result.h"

#include <cstdint>
#include <optional>
#include <sqlite3.h>
#include <string>

namespace mobmatter::common::persistence::sqlite {

class Connection;

class Statement final {
public:
    Statement(Statement&& other) noexcept;
    Statement& operator=(Statement&& other) = delete;

    Statement(const Statement& other) = delete;
    Statement& operator=(const Statement& other) = delete;

    ~Statement();

    void bind(int param, const std::string& value);
    void bind(int param, const char* value);
    void bind(int param, const void* buf, int size);
    void bind(int param, bool value);
    void bind(int param, int32_t value);
    void bind(int param, uint32_t value);
    void bind(int param, int64_t value);

    template <typename T>
    void bind(const int param, const std::optional<T>& op)
    {
        if (!op) {
            sqlite3_bind_null(mStmt, param);
            return;
        }

        bind(param, *op);
    }

    std::string columnAsString(int index) const;
    const void* columnAsBlob(int index) const;
    bool columnAsBool(int index) const;
    int8_t columnAsInt8(int index) const;
    int16_t columnAsInt16(int index) const;
    int32_t columnAsInt32(int index) const;
    int64_t columnAsInt64(int index) const;
    uint8_t columnAsUint8(int index) const;
    uint16_t columnAsUint16(int index) const;
    uint32_t columnAsUint32(int index) const;
    bool isColumnNull(int index) const;
    int columnSize(int index) const;
    int changes() const;
    Result<bool> fetch();
    Result<> exec();

private:
    friend class Connection; // calls constructor

    sqlite3_stmt* mStmt;

    explicit Statement(sqlite3_stmt* stmt);
};

}
