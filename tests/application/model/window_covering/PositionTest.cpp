#include "application/model/window_covering/Position.h"

#include <gtest/gtest.h>

using mmbridge::application::model::Percent;
using mmbridge::application::model::window_covering::Position;

TEST(PositionTest, FullyOpen)
{
    auto position = Position::fullyOpen();

    EXPECT_EQ(Percent::max(), position.openPercent());
    EXPECT_EQ(Percent::min(), position.closedPercent());
}

TEST(PositionTest, FullyClosed)
{
    auto position = Position::fullyClosed();

    EXPECT_EQ(Percent::max(), position.closedPercent());
    EXPECT_EQ(Percent::min(), position.openPercent());
}

TEST(PositionTest, OpenAt)
{
    auto position = Position::open(*Percent::from(23));

    EXPECT_EQ(23, position.openPercent().value());
    EXPECT_EQ(77, position.closedPercent().value());
}

TEST(PositionTest, ClosedAt)
{
    auto position = Position::closed(*Percent::from(23));

    EXPECT_EQ(23, position.closedPercent().value());
    EXPECT_EQ(77, position.openPercent().value());
}

TEST(PositionTest, IsAndIsNotFullyOpen)
{
    EXPECT_TRUE(Position::fullyOpen().isFullyOpen());
    EXPECT_TRUE(Position::open(*Percent::from(100)).isFullyOpen());
    EXPECT_FALSE(Position::open(*Percent::from100ths(9999)).isFullyOpen());
    EXPECT_FALSE(Position::fullyClosed().isFullyOpen());
}

TEST(PositionTest, IsAndIsNotFullyClosed)
{
    EXPECT_TRUE(Position::fullyClosed().isFullyClosed());
    EXPECT_TRUE(Position::closed(*Percent::from(100)).isFullyClosed());
    EXPECT_FALSE(Position::closed(*Percent::from100ths(9999)).isFullyClosed());
    EXPECT_FALSE(Position::fullyOpen().isFullyClosed());
}

TEST(PositionTest, IsAndIsNotOpen)
{
    EXPECT_TRUE(Position::fullyOpen().isOpen());
    EXPECT_TRUE(Position::open(*Percent::from100ths(1)).isOpen());
    EXPECT_FALSE(Position::fullyClosed().isOpen());
}

TEST(PositionTest, Compare)
{
    EXPECT_EQ(Position::open(*Percent::from(10)), Position::closed(*Percent::from(90)));
    EXPECT_NE(Position::open(*Percent::from(10)), Position::closed(*Percent::from(10)));
}
