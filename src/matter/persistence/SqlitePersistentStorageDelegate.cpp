#include "SqlitePersistentStorageDelegate.h"
#include <cstring>

namespace sqlite = mobmatter::common::persistence::sqlite;

namespace mobmatter::matter::persistence {

CHIP_ERROR SqlitePersistentStorageDelegate::Init(sqlite::Connection* conn, logging::Logger* logger)
{
    if (nullptr == conn) {
        return CHIP_ERROR_INVALID_ARGUMENT;
    }
    if (nullptr == logger) {
        return CHIP_ERROR_INVALID_ARGUMENT;
    }

    mConn = conn;
    mLogger = logger;

    return CHIP_NO_ERROR;
}

CHIP_ERROR SqlitePersistentStorageDelegate::SyncGetKeyValue(const char* key, void* buffer, uint16_t& size)
{
    auto stmt = mConn->prepare("SELECT value FROM kvs WHERE key = ?");
    stmt->bind(1, key);

    auto r = stmt->fetch();

    if (!r) {
        mLogger->error("KVS get key failed: %s", r.error().message().c_str());
    }

    if (!r.value()) {
        return CHIP_ERROR_PERSISTED_STORAGE_VALUE_NOT_FOUND;
    }

    auto storedSize = stmt->columnSize(0);

    if (storedSize > size) {
        return CHIP_ERROR_BUFFER_TOO_SMALL;
    }

    if (storedSize < size) {
        size = static_cast<uint16_t>(storedSize);
    }

    memcpy(buffer, stmt->columnAsBlob(0), size);

    return CHIP_NO_ERROR;
}

CHIP_ERROR SqlitePersistentStorageDelegate::SyncSetKeyValue(const char* key, const void* value, uint16_t size)
{
    auto stmt = mConn->prepare("INSERT OR REPLACE INTO kvs (key, value) VALUES (?, ?)");

    stmt->bind(1, key);
    stmt->bind(2, value, size);

    if (auto r = stmt->exec(); !r) {
        mLogger->error("KVS set key failed: %s", r.error().message().c_str());
        return CHIP_ERROR_PERSISTED_STORAGE_FAILED;
    }

    return CHIP_NO_ERROR;
}

CHIP_ERROR SqlitePersistentStorageDelegate::SyncDeleteKeyValue(const char* key)
{
    auto stmt = mConn->prepare("DELETE FROM kvs WHERE key = ?");
    stmt->bind(1, key);

    if (auto r = stmt->exec(); !r) {
        mLogger->error("KVS delete key failed: %s", r.error().message().c_str());
        return CHIP_ERROR_PERSISTED_STORAGE_FAILED;
    }

    return CHIP_NO_ERROR;
}

bool SqlitePersistentStorageDelegate::SyncDoesKeyExist(const char* key)
{
    auto stmt = mConn->prepare("SELECT 1 FROM kvs WHERE key = ?");
    stmt->bind(1, key);

    auto r = stmt->fetch();

    if (!r) {
        mLogger->error("KVS does key exist failed: %s", r.error().message().c_str());
        return false;
    }

    return *r;
}

}
