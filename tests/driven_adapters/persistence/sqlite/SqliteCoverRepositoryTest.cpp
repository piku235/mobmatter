#include "driven_adapters/persistence/sqlite/SqliteCoverRepository.h"
#include "SqliteDatabaseSchema.h"
#include "application/model/window_covering/Cover.h"
#include "common/persistence/sqlite/Connection.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

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

    ASSERT_TRUE(savedCover.has_value());
    ASSERT_EQ(cover.endpointId(), savedCover->endpointId());
    ASSERT_EQ(cover.mobilusDeviceId(), savedCover->mobilusDeviceId());
    ASSERT_EQ(cover.isReachable(), savedCover->isReachable());
    ASSERT_EQ(cover.name(), savedCover->name());
    ASSERT_EQ(cover.liftState().status(), savedCover->liftState().status());
    ASSERT_EQ(cover.liftState().motion(), savedCover->liftState().motion());
    ASSERT_EQ(cover.liftState().targetPosition(), savedCover->liftState().targetPosition());
    ASSERT_EQ(cover.liftState().currentPosition(), savedCover->liftState().currentPosition());
    ASSERT_EQ(cover.specification(), savedCover->specification());
}

TEST_F(SqliteCoverRepositoryTest, Removes)
{
    auto cover = sensoStub();

    coverRepository.save(cover);
    ASSERT_TRUE(coverRepository.find(cover.endpointId()));

    coverRepository.remove(cover);
    ASSERT_FALSE(coverRepository.find(cover.endpointId()));
}

TEST_F(SqliteCoverRepositoryTest, DoesNotFind)
{
    auto cover = sensoStub();
    coverRepository.save(cover);

    auto foundCover = coverRepository.find(11);

    ASSERT_FALSE(foundCover.has_value());
}

TEST_F(SqliteCoverRepositoryTest, FindsAndDoesNotFindOfMobilusDeviceId)
{
    auto cover = sensoStub();
    coverRepository.save(cover);

    auto foundCover = coverRepository.findOfMobilusDeviceId(cover.mobilusDeviceId());

    ASSERT_TRUE(foundCover.has_value());
    ASSERT_EQ(cover, foundCover);

    ASSERT_FALSE(coverRepository.findOfMobilusDeviceId(12));
}

TEST_F(SqliteCoverRepositoryTest, FindsAll)
{
    std::vector<EndpointId> expectedCovers = { 1, 11 };

    coverRepository.save(sensoStub());
    coverRepository.save(cmrStub());

    auto covers = coverRepository.all();

    ASSERT_EQ(2u, covers.size());

    for (auto cover : covers) {
        ASSERT_THAT(expectedCovers, Contains(cover.endpointId()));
    }
}
