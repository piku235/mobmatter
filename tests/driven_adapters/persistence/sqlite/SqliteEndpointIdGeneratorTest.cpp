#include "driven_adapters/persistence/sqlite/SqliteEndpointIdGenerator.h"
#include "SqliteDatabaseSchema.h"
#include "application/model/window_covering/Cover.h"
#include "common/persistence/sqlite/Connection.h"

#include <gtest/gtest.h>

using namespace mobmatter::driven_adapters::persistence::sqlite;
using mobmatter::application::model::EndpointId;

class SqliteEndpointIdGeneratorTest : public ::testing::Test {
protected:
    sqlite::Connection conn;
    SqliteEndpointIdGenerator endpointIdGenerator;

    SqliteEndpointIdGeneratorTest()
        : conn(*sqlite::Connection::inMemory())
        , endpointIdGenerator(10u, conn)
    {
        conn.exec(kDatabaseSchema);
    }
};

TEST_F(SqliteEndpointIdGeneratorTest, GeneratesNextEndpointId)
{
    ASSERT_EQ(10u, endpointIdGenerator.next());
    ASSERT_EQ(11u, endpointIdGenerator.next());
    ASSERT_EQ(12u, endpointIdGenerator.next());
}
