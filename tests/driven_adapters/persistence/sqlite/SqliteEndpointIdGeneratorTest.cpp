#include "driven_adapters/persistence/sqlite/SqliteEndpointIdGenerator.h"
#include "SqliteDatabaseSchema.h"
#include "application/model/window_covering/Cover.h"
#include "common/persistence/sqlite/Connection.h"

#include <gtest/gtest.h>

using namespace mmbridge::driven_adapters::persistence::sqlite;
using mmbridge::application::model::EndpointId;

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
    EXPECT_EQ(10u, endpointIdGenerator.next());
    EXPECT_EQ(11u, endpointIdGenerator.next());
    EXPECT_EQ(12u, endpointIdGenerator.next());
}
