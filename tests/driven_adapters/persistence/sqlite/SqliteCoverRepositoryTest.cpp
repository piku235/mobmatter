#include "driven_adapters/persistence/sqlite/SqliteCoverRepository.h"
#include "SqliteDatabaseSchema.h"
#include "application/model/window_covering/Cover.h"
#include "common/logging/Logger.h"
#include "common/persistence/sqlite/Connection.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace mobmatter::application::model;
using namespace mobmatter::application::model::window_covering;
using namespace mobmatter::driven_adapters::persistence::sqlite;
using namespace mobmatter::common::logging;

namespace {

auto liftAndTiltCover()
{
    return Cover::add(1, 11, CoverSpecification::SensoZ(), "SensoZ", PositionState::at(Position::fullyOpen()), PositionState::at(Position::fullyClosed()));
}

auto liftCover()
{
    return Cover::add(2, 12, CoverSpecification::Senso(), "Senso", PositionState::at(Position::fullyOpen()), PositionState::unavailable());
}

}

class SqliteCoverRepositoryTest : public ::testing::Test {
protected:
    sqlite::Connection conn;
    SqliteCoverRepository coverRepository;

    SqliteCoverRepositoryTest()
        : conn(*sqlite::Connection::inMemory())
        , coverRepository(conn, Logger::noop())
    {
        (void)conn.exec(kDatabaseSchema);
    }
};

TEST_F(SqliteCoverRepositoryTest, Saves)
{
    for (auto& cover : { liftAndTiltCover(), liftCover() }) {
        coverRepository.save(cover);
        auto savedCover = coverRepository.find(cover.endpointId());

        ASSERT_TRUE(savedCover.has_value());
        ASSERT_EQ(cover.endpointId(), savedCover->endpointId());
        ASSERT_EQ(cover.mobilusDeviceId(), savedCover->mobilusDeviceId());
        ASSERT_EQ(cover.specification(), savedCover->specification());
        ASSERT_EQ(cover.isReachable(), savedCover->isReachable());
        ASSERT_EQ(cover.name(), savedCover->name());
        ASSERT_EQ(cover.liftState().status(), savedCover->liftState().status());
        ASSERT_EQ(cover.liftState().motion(), savedCover->liftState().motion());
        ASSERT_EQ(cover.liftState().targetPosition(), savedCover->liftState().targetPosition());
        ASSERT_EQ(cover.liftState().currentPosition(), savedCover->liftState().currentPosition());
        ASSERT_EQ(cover.tiltState().status(), savedCover->tiltState().status());
        ASSERT_EQ(cover.tiltState().motion(), savedCover->tiltState().motion());
        ASSERT_EQ(cover.tiltState().targetPosition(), savedCover->tiltState().targetPosition());
        ASSERT_EQ(cover.tiltState().currentPosition(), savedCover->tiltState().currentPosition());
    }
}

TEST_F(SqliteCoverRepositoryTest, Removes)
{
    auto cover = liftCover();

    coverRepository.save(cover);
    ASSERT_TRUE(coverRepository.find(cover.endpointId()));

    coverRepository.remove(cover);
    ASSERT_FALSE(coverRepository.find(cover.endpointId()));
}

TEST_F(SqliteCoverRepositoryTest, FindsAndDoesNotFindOfMobilusDeviceId)
{
    auto cover = liftCover();
    coverRepository.save(cover);

    auto foundCover = coverRepository.findOfMobilusDeviceId(cover.mobilusDeviceId());

    ASSERT_TRUE(foundCover.has_value());
    ASSERT_EQ(cover, foundCover);

    ASSERT_FALSE(coverRepository.findOfMobilusDeviceId(123));
}

TEST_F(SqliteCoverRepositoryTest, FindsAll)
{
    std::vector<EndpointId> expectedCovers = { 1, 2 };

    coverRepository.save(liftAndTiltCover());
    coverRepository.save(liftCover());

    auto covers = coverRepository.all();

    ASSERT_EQ(2u, covers.size());

    for (auto cover : covers) {
        ASSERT_THAT(expectedCovers, ::testing::Contains(cover.endpointId()));
    }
}
