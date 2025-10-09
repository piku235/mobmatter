#include "driven_adapters/persistence/sqlite/SqliteEndpointIdGenerator.h"
#include "SqliteDatabaseSchema.h"
#include "application/model/window_covering/Cover.h"
#include "common/logging/Logger.h"
#include "common/persistence/sqlite/Connection.h"

#include <gtest/gtest.h>

using namespace mobmatter::driven_adapters::persistence::sqlite;
using namespace mobmatter::common::logging;
using mobmatter::application::model::EndpointId;

class SqliteEndpointIdGeneratorTest : public ::testing::Test {
protected:
    sqlite::Connection conn;
    SqliteEndpointIdGenerator endpointIdGenerator;

    SqliteEndpointIdGeneratorTest()
        : conn(*sqlite::Connection::inMemory())
        , endpointIdGenerator(10u, conn, Logger::noop())
    {
        (void)conn.exec(kDatabaseSchema);
    }
};

TEST_F(SqliteEndpointIdGeneratorTest, GeneratesNextEndpointId)
{
    auto r = endpointIdGenerator.next();
    ASSERT_TRUE(r.has_value());
    ASSERT_EQ(10u, *r);

    r = endpointIdGenerator.next();
    ASSERT_TRUE(r.has_value());
    ASSERT_EQ(11u, *r);
}
