#include "application/model/window_covering/Position.h"

#include <gtest/gtest.h>

using mmbridge::application::model::Percent;
using mmbridge::application::model::window_covering::Position;

TEST(PositionTest, FullyOpen)
{
    auto position = Position::fullyOpen();

    ASSERT_EQ(Percent::max(), position.openPercent());
    ASSERT_EQ(Percent::min(), position.closedPercent());
}

TEST(PositionTest, FullyClosed)
{
    auto position = Position::fullyClosed();

    ASSERT_EQ(Percent::max(), position.closedPercent());
    ASSERT_EQ(Percent::min(), position.openPercent());
}

TEST(PositionTest, OpenAt)
{
    auto position = Position::open(*Percent::from(23));

    ASSERT_EQ(23, position.openPercent().value());
    ASSERT_EQ(77, position.closedPercent().value());
}

TEST(PositionTest, ClosedAt)
{
    auto position = Position::closed(*Percent::from(23));

    ASSERT_EQ(23, position.closedPercent().value());
    ASSERT_EQ(77, position.openPercent().value());
}

TEST(PositionTest, IsAndIsNotFullyOpen)
{
    ASSERT_TRUE(Position::fullyOpen().isFullyOpen());
    ASSERT_TRUE(Position::open(*Percent::from(100)).isFullyOpen());
    ASSERT_FALSE(Position::open(*Percent::from100ths(9999)).isFullyOpen());
    ASSERT_FALSE(Position::fullyClosed().isFullyOpen());
}

TEST(PositionTest, IsAndIsNotFullyClosed)
{
    ASSERT_TRUE(Position::fullyClosed().isFullyClosed());
    ASSERT_TRUE(Position::closed(*Percent::from(100)).isFullyClosed());
    ASSERT_FALSE(Position::closed(*Percent::from100ths(9999)).isFullyClosed());
    ASSERT_FALSE(Position::fullyOpen().isFullyClosed());
}

TEST(PositionTest, IsAndIsNotOpen)
{
    ASSERT_TRUE(Position::fullyOpen().isOpen());
    ASSERT_TRUE(Position::open(*Percent::from100ths(1)).isOpen());
    ASSERT_FALSE(Position::fullyClosed().isOpen());
}

TEST(PositionTest, Compare)
{
    ASSERT_EQ(Position::open(*Percent::from(10)), Position::closed(*Percent::from(90)));
    ASSERT_NE(Position::open(*Percent::from(10)), Position::closed(*Percent::from(10)));
}
