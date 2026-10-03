#pragma once

constexpr char kDatabaseSchema[] = "BEGIN;"
                                   "CREATE TABLE sequence (name TEXT PRIMARY KEY, value INTEGER NOT NULL);"
                                   "CREATE TABLE kvs (key TEXT PRIMARY KEY, value BLOB NOT NULL);"
                                   "CREATE TABLE cover (endpoint_id INTEGER PRIMARY KEY, mobilus_device_id INTEGER UNIQUE NOT NULL, spec_mobilus_device_type INTEGER NOT NULL, reachable BOOLEAN NOT NULL, name TEXT NOT NULL, motion_fault INTEGER, lift_status INTEGER NOT NULL, lift_motion INTEGER NOT NULL, lift_target_position INTEGER NOT NULL, lift_current_position INTEGER NOT NULL, tilt_status INTEGER, tilt_motion INTEGER, tilt_target_position INTEGER, tilt_current_position INTEGER);"
                                   "CREATE TABLE switch (endpoint_id INTEGER PRIMARY KEY, mobilus_device_id INTEGER UNIQUE NOT NULL, reachable BOOLEAN NOT NULL, on_off BOOLEAN NOT NULL, name TEXT NOT NULL);"
                                   "PRAGMA user_version = 1;"
                                   "COMMIT;";

constexpr char kMigrationV1[] = "CREATE TABLE cover_new (endpoint_id INTEGER PRIMARY KEY, mobilus_device_id INTEGER UNIQUE NOT NULL, spec_mobilus_device_type INTEGER NOT NULL, reachable BOOLEAN NOT NULL, name TEXT NOT NULL, motion_fault INTEGER, lift_status INTEGER NOT NULL, lift_motion INTEGER NOT NULL, lift_target_position INTEGER NOT NULL, lift_current_position INTEGER NOT NULL, tilt_status INTEGER, tilt_motion INTEGER, tilt_target_position INTEGER, tilt_current_position INTEGER);"
                                "INSERT INTO cover_new SELECT endpoint_id, mobilus_device_id, spec_mobilus_device_type, reachable, name, null, lift_status, lift_motion, lift_target_position, lift_current_position FROM cover;"
                                "DROP TABLE cover;"
                                "ALTER TABLE cover_new RENAME TO cover;"
                                "CREATE TABLE switch (endpoint_id INTEGER PRIMARY KEY, mobilus_device_id INTEGER UNIQUE NOT NULL, reachable BOOLEAN NOT NULL, on_off BOOLEAN NOT NULL, name TEXT NOT NULL);"
                                "VACUUM;";

inline const char* kMigrations[] = {
    kMigrationV1,
};
