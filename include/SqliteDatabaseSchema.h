#pragma once

constexpr char kDatabaseSchema[] = "BEGIN;"
                                   "CREATE TABLE sequence (name TEXT PRIMARY KEY, value INTEGER NOT NULL);"
                                   "CREATE TABLE kvs (key TEXT PRIMARY KEY, value BLOB NOT NULL);"
                                   "CREATE TABLE cover (endpoint_id INTEGER PRIMARY KEY, mobilus_device_id INTEGER UNIQUE NOT NULL, spec_mobilus_device_type INTEGER NOT NULL, reachable BOOLEAN NOT NULL, name TEXT NOT NULL, lift_status INTEGER NOT NULL, lift_motion INTEGER NOT NULL, lift_target_position INTEGER, lift_current_position INTEGER, tilt_status INTEGER NOT NULL, tilt_motion INTEGER NOT NULL, tilt_target_position INTEGER, tilt_current_position INTEGER);"
                                   "CREATE TABLE switch (endpoint_id INTEGER PRIMARY KEY, mobilus_device_id INTEGER UNIQUE NOT NULL, state INTEGER NOT NULL, name TEXT NOT NULL);"
                                   "PRAGMA user_version = 1;"
                                   "COMMIT;";

constexpr char kMigrationV1[] = "ALTER TABLE cover ADD COLUMN tilt_status INTEGER NOT NULL DEFAULT 0;"
                                "ALTER TABLE cover ADD COLUMN tilt_motion INTEGER NOT NULL DEFAULT 0;"
                                "ALTER TABLE cover ADD COLUMN tilt_target_position INTEGER;"
                                "ALTER TABLE cover ADD COLUMN tilt_current_position INTEGER;"
                                "ALTER TABLE cover DROP unique_id;"
                                "CREATE TABLE switch (endpoint_id INTEGER PRIMARY KEY, mobilus_device_id INTEGER UNIQUE NOT NULL, state INTEGER NOT NULL, name TEXT NOT NULL);";

inline const char* kMigrations[] = {
    kMigrationV1,
};
