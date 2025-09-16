#pragma once

#define ZCL_AGGREGATOR_ENDPOINT_ID 1u

#define ZCL_INITIAL_DYNAMIC_ENDPOINT_ID 2u

#define LOG_IDENT "mobmatter"

// Database
#ifndef DATABASE_FILE
#define DATABASE_FILE "/tmp/mobmatter.db"
#endif

#ifndef DATABASE_PRAGMA_SQL
#define DATABASE_PRAGMA_SQL ""
#endif

// Mobilus
#ifndef MOBILUS_HOST
#define MOBILUS_HOST "127.0.0.1"
#endif

#ifndef MOBILUS_PORT
#define MOBILUS_PORT 8883
#endif

#ifndef MOBILUS_CA_FILE
#define MOBILUS_CA_FILE ""
#endif

#include <string>

struct AppConfig {
    std::string mobilusUsername;
    std::string mobilusPassword;
};
