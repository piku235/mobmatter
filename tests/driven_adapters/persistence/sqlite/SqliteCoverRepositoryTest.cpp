#include "driven_adapters/persistence/sqlite/SqliteCoverRepository.h"
#include "SqliteDatabaseSchema.h"
#include "application/model/window_covering/Cover.h"
#include "common/persistence/sqlite/Connection.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace mmbridge::application::model;
using namespace mmbridge::application::model::window_covering;
using namespace mmbridge::driven_adapters::persistence::sqlite;
using testing::Contains;

namespace {

auto sensoStub()
{
    return Cover::add(1, 2, "senso", PositionState::at(Position::fullyOpen()), CoverSpecification::Senso());
}

auto cmrStub()
{
    return Cover::add(11, 12, "cmr", PositionState::at(Position::fullyClosed()), CoverSpecification::Cmr());
}

}

class SqliteCoverRepositoryTest : public ::testing::Test {
protected:
    sqlite::Connection conn;
    SqliteCoverRepository coverRepository;

    SqliteCoverRepositoryTest()
        : conn(*sqlite::Connection::inMemory())
        , coverRepository(conn)
    {
        conn.exec(kDatabaseSchema);
    }
};

TEST_F(SqliteCoverRepositoryTest, Saves)
{
    auto cover = sensoStub();

    coverRepository.save(cover);
    auto savedCover = coverRepository.find(cover.endpointId());

    EXPECT_TRUE(savedCover.has_value());
    EXPECT_EQ(cover.endpointId(), savedCover->endpointId());
    EXPECT_EQ(cover.mobilusDeviceId(), savedCover->mobilusDeviceId());
    EXPECT_EQ(cover.isReachable(), savedCover->isReachable());
    EXPECT_EQ(cover.name(), savedCover->name());
    EXPECT_EQ(cover.liftState().status(), savedCover->liftState().status());
    EXPECT_EQ(cover.liftState().motion(), savedCover->liftState().motion());
    EXPECT_EQ(cover.liftState().targetPosition(), savedCover->liftState().targetPosition());
    EXPECT_EQ(cover.liftState().currentPosition(), savedCover->liftState().currentPosition());
    EXPECT_EQ(cover.specification(), savedCover->specification());
}

TEST_F(SqliteCoverRepositoryTest, Removes)
{
    auto cover = sensoStub();
    coverRepository.save(cover);

    coverRepository.remove(cover);

    EXPECT_FALSE(coverRepository.find(cover.endpointId()));
}

TEST_F(SqliteCoverRepositoryTest, DoesNotFind)
{
    auto cover = sensoStub();
    coverRepository.save(cover);

    auto foundCover = coverRepository.find(11);

    EXPECT_FALSE(foundCover.has_value());
}

TEST_F(SqliteCoverRepositoryTest, FindsAndDoesNotFindOfMobilusDeviceId)
{
    auto cover = sensoStub();
    coverRepository.save(cover);

    auto foundCover = coverRepository.findOfMobilusDeviceId(cover.mobilusDeviceId());

    EXPECT_TRUE(foundCover.has_value());
    EXPECT_EQ(cover, foundCover);

    EXPECT_FALSE(coverRepository.findOfMobilusDeviceId(12));
}

TEST_F(SqliteCoverRepositoryTest, FindsAll)
{
    std::vector<EndpointId> expectedCovers = { 1, 11 };

    coverRepository.save(sensoStub());
    coverRepository.save(cmrStub());

    auto covers = coverRepository.all();

    EXPECT_EQ(2u, covers.size());

    for (auto cover : covers) {
        EXPECT_THAT(expectedCovers, ::testing::Contains(cover.endpointId()));
    }
}
