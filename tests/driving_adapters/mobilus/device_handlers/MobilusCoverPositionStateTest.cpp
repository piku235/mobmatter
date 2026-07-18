#include "driving_adapters/mobilus/device_handlers/MobilusCoverPositionState.h"
#include "application/model/Percent.h"
#include "application/model/window_covering/Position.h"

#include <gtest/gtest.h>

#include <string>
#include <tuple>

using mobmatter::application::model::Percent;
using mobmatter::application::model::window_covering::Position;
using mobmatter::driving_adapters::mobilus::device_handlers::MobilusCoverPositionState;
using testing::TestWithParam;
using testing::Values;

namespace {

class MobilusCoverLiftAndTiltPositionStateTest : public TestWithParam<std::tuple<std::string, Position, Position>> { };

class MobilusCoverLiftPositionStateTest : public TestWithParam<std::tuple<std::string, Position>> { };

class MobilusCoverTiltPositionStateTest : public TestWithParam<std::tuple<std::string, Position>> { };

class MobilusCoverPositionStateTest : public TestWithParam<std::string> { };

}

TEST_P(MobilusCoverLiftAndTiltPositionStateTest, Parses)
{
    auto [value, expectedLiftPosition, expectedTiltPosition] = GetParam();

    auto positionState = MobilusCoverPositionState::parse(value);

    ASSERT_TRUE(positionState.isValid());
    ASSERT_TRUE(positionState.liftPosition.has_value());
    ASSERT_TRUE(positionState.tiltPosition.has_value());
    ASSERT_EQ(expectedLiftPosition, *positionState.liftPosition);
    ASSERT_EQ(expectedTiltPosition, *positionState.tiltPosition);
}

TEST_P(MobilusCoverLiftPositionStateTest, Parses)
{
    auto [value, expectedLiftPosition] = GetParam();

    auto positionState = MobilusCoverPositionState::parse(value);

    ASSERT_TRUE(positionState.isValid());
    ASSERT_TRUE(positionState.liftPosition.has_value());
    ASSERT_FALSE(positionState.tiltPosition.has_value());
    ASSERT_EQ(expectedLiftPosition, *positionState.liftPosition);
}

TEST_P(MobilusCoverTiltPositionStateTest, Parses)
{
    auto [value, expectedTiltPosition] = GetParam();

    auto positionState = MobilusCoverPositionState::parse(value);

    ASSERT_TRUE(positionState.isValid());
    ASSERT_TRUE(positionState.tiltPosition.has_value());
    ASSERT_FALSE(positionState.liftPosition.has_value());
    ASSERT_EQ(expectedTiltPosition, *positionState.tiltPosition);
}

TEST_P(MobilusCoverPositionStateTest, ParseFails)
{
    auto positionState = MobilusCoverPositionState::parse(GetParam());

    ASSERT_FALSE(positionState.isValid());
    ASSERT_FALSE(positionState.liftPosition.has_value());
    ASSERT_FALSE(positionState.tiltPosition.has_value());
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(Positions, MobilusCoverLiftAndTiltPositionStateTest, Values(
    std::make_tuple("100%:0$", Position::fullyOpen(), Position::fullyClosed()),
    std::make_tuple("0%:100$", Position::fullyClosed(), Position::fullyOpen()),
    std::make_tuple("12%:23$", Position::open(*Percent::from(12)), Position::open(*Percent::from(23))),
    std::make_tuple("DOWN:23$", Position::fullyClosed(), Position::open(*Percent::from(23)))
));
INSTANTIATE_TEST_SUITE_P(Positions, MobilusCoverLiftPositionStateTest, Values(
    std::make_tuple("100%", Position::fullyOpen()),
    std::make_tuple("0%", Position::fullyClosed()),
    std::make_tuple("12%", Position::open(*Percent::from(12)))
));
INSTANTIATE_TEST_SUITE_P(Positions, MobilusCoverTiltPositionStateTest, Values(
    std::make_tuple("100$", Position::fullyOpen()),
    std::make_tuple("0$", Position::fullyClosed()),
    std::make_tuple("12$", Position::open(*Percent::from(12)))
));
INSTANTIATE_TEST_SUITE_P(InvalidPositions, MobilusCoverPositionStateTest, Values(
    "",
    "0",
    "UP",
    "DOWN",
    "%100%",
    "100%%",
    "101%",
    "$100$",
    "100$$",
    "23$:12%",
    "UP:100$",
    "100%:UP",
    "0%:101$"
));
// clang-format on
