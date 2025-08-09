#include "application/model/window_covering/PositionState.h"

#include <gtest/gtest.h>

using namespace mmbridge::application::model::window_covering;

TEST(PositionStateTest, Unavailable) {
    auto state = PositionState::unavailable();

    EXPECT_EQ(PositionStatus::Unavailable, state.status());
    EXPECT_EQ(CoverMotion::NotMoving, state.motion());
    EXPECT_FALSE(state.targetPosition().has_value());
    EXPECT_FALSE(state.currentPosition().has_value());
}

TEST(PositionStateTest, AtPosition) {
    auto state = PositionState::at(Position::fullyOpen());

    EXPECT_EQ(PositionStatus::Idle, state.status());
    EXPECT_EQ(CoverMotion::NotMoving, state.motion());
    EXPECT_EQ(Position::fullyOpen(), *state.targetPosition());
    EXPECT_EQ(Position::fullyOpen(), *state.currentPosition());
}

TEST(PositionStateTest, Restores) {
    auto state = PositionState::restore(PositionStatus::Moving, CoverMotion::Opening, Position::fullyOpen(), Position::fullyClosed());

    EXPECT_EQ(PositionStatus::Moving, state.status());
    EXPECT_EQ(CoverMotion::Opening, state.motion());
    EXPECT_EQ(Position::fullyOpen(), *state.targetPosition());
    EXPECT_EQ(Position::fullyClosed(), *state.currentPosition());
}

TEST(PositionStateTest, MovesToOpeningPosition) {
    auto state = PositionState::at(Position::fullyClosed());
    
    auto newState = state.movingTo(Position::fullyOpen());

    EXPECT_EQ(PositionStatus::Moving, newState.status());
    EXPECT_EQ(CoverMotion::Opening, newState.motion());
    EXPECT_EQ(Position::fullyOpen(), *newState.targetPosition());
    EXPECT_EQ(Position::fullyClosed(), *newState.currentPosition());
}

TEST(PositionStateTest, MovesToClosedPosition) {
    auto state = PositionState::at(Position::fullyOpen());
    
    auto newState = state.movingTo(Position::fullyClosed());

    EXPECT_EQ(PositionStatus::Moving, newState.status());
    EXPECT_EQ(CoverMotion::Closing, newState.motion());
    EXPECT_EQ(Position::fullyClosed(), *newState.targetPosition());
    EXPECT_EQ(Position::fullyOpen(), *newState.currentPosition());
}

TEST(PositionStateTest, RequestsMoveToPosition) {
    auto state = PositionState::at(Position::fullyOpen());
    
    auto newState = state.requestMoveTo(Position::fullyClosed());

    EXPECT_EQ(PositionStatus::Requested, newState.status());
    EXPECT_EQ(CoverMotion::NotMoving, newState.motion());
    EXPECT_EQ(Position::fullyClosed(), *newState.targetPosition());
    EXPECT_EQ(Position::fullyOpen(), *newState.currentPosition());
}

TEST(PositionStateTest, StopsOpening) {
    auto state = PositionState::at(Position::fullyClosed())
        .movingTo(Position::fullyOpen());

    auto newState = state.stop();

    EXPECT_EQ(PositionStatus::Stopping, newState.status());
    EXPECT_EQ(CoverMotion::Opening, newState.motion());
    EXPECT_EQ(Position::fullyOpen(), *newState.targetPosition());
    EXPECT_EQ(Position::fullyClosed(), *newState.currentPosition());
}

TEST(PositionStateTest, StopsClosing) {
    auto state = PositionState::at(Position::fullyOpen())
        .movingTo(Position::fullyClosed());

    auto newState = state.stop();

    EXPECT_EQ(PositionStatus::Stopping, newState.status());
    EXPECT_EQ(CoverMotion::Closing, newState.motion());
    EXPECT_EQ(Position::fullyClosed(), *newState.targetPosition());
    EXPECT_EQ(Position::fullyOpen(), *newState.currentPosition());
}

TEST(PositionStateTest, Resets) {
    auto state = PositionState::at(Position::fullyOpen())
        .movingTo(Position::fullyClosed());

    auto newState = state.reset();

    EXPECT_EQ(PositionStatus::Idle, newState.status());
    EXPECT_EQ(CoverMotion::NotMoving, newState.motion());
    EXPECT_EQ(Position::fullyOpen(), *newState.targetPosition());
    EXPECT_EQ(Position::fullyOpen(), *newState.currentPosition());
}
