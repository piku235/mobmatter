#pragma once

constexpr char kDatabaseSchema[] = "BEGIN;"
                                   "CREATE TABLE sequence (name TEXT PRIMARY KEY, value INTEGER NOT NULL);"
                                   "CREATE TABLE kvs (key TEXT PRIMARY KEY, value BLOB NOT NULL);"
                                   "CREATE TABLE cover (endpoint_id INTEGER PRIMARY KEY, mobilus_device_id INTEGER UNIQUE NOT NULL, unique_id TEXT UNIQUE NOT NULL, reachable BOOLEAN NOT NULL, name TEXT NOT NULL, lift_status INTEGER NOT NULL, lift_motion INTEGER NOT NULL, lift_target_position INTEGER, lift_current_position INTEGER, spec_mobilus_device_type INTEGER NOT NULL);"
                                   "COMMIT;";
