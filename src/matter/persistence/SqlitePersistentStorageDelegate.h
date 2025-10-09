#pragma once

#include "common/logging/Logger.h"
#include "common/persistence/sqlite/Connection.h"

#include <lib/core/CHIPPersistentStorageDelegate.h>

namespace mobmatter::matter::persistence {

namespace sqlite = mobmatter::common::persistence::sqlite;
namespace logging = mobmatter::common::logging;

class SqlitePersistentStorageDelegate final : public chip::PersistentStorageDelegate {
public:
    CHIP_ERROR Init(sqlite::Connection* conn, logging::Logger* logger);
    CHIP_ERROR SyncGetKeyValue(const char* key, void* buffer, uint16_t& size) override;
    CHIP_ERROR SyncSetKeyValue(const char* key, const void* value, uint16_t size) override;
    CHIP_ERROR SyncDeleteKeyValue(const char* key) override;
    bool SyncDoesKeyExist(const char* key) override;

private:
    sqlite::Connection* mConn;
    logging::Logger* mLogger;
};

}
