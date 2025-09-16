#include "PositionState.h"

namespace mobmatter::application::model::window_covering {

PositionState PositionState::unavailable()
{
    return { PositionStatus::Unavailable, CoverMotion::NotMoving, std::nullopt, std::nullopt };
}

PositionState PositionState::at(Position position)
{
    return { PositionStatus::Idle, CoverMotion::NotMoving, position, position };
}

PositionState PositionState::restore(PositionStatus status, CoverMotion motion, std::optional<Position> targetPosition, std::optional<Position> currentPosition)
{
    return { status, motion, std::move(targetPosition), std::move(currentPosition) };
}

PositionState PositionState::movingTo(Position position) const
{
    return {
        mCurrentPosition != position ? PositionStatus::Moving : PositionStatus::Idle,
        position.openPercent() > mCurrentPosition->openPercent() ? CoverMotion::Opening : (position.openPercent() < mCurrentPosition->openPercent() ? CoverMotion::Closing : CoverMotion::NotMoving),
        position,
        mCurrentPosition,
    };
}

PositionState PositionState::requestMoveTo(Position position) const
{
    return { PositionStatus::Requested, CoverMotion::NotMoving, position, mCurrentPosition };
}

PositionState PositionState::stop() const
{
    return { PositionStatus::Stopping, mMotion, mTargetPosition, mCurrentPosition };
}

PositionState PositionState::reset() const
{
    return { PositionStatus::Idle, CoverMotion::NotMoving, mCurrentPosition, mCurrentPosition };
}

PositionState::PositionState(PositionStatus status, CoverMotion motion, std::optional<Position> targetPosition, std::optional<Position> currentPosition)
    : mStatus(status)
    , mMotion(motion)
    , mTargetPosition(std::move(targetPosition))
    , mCurrentPosition(std::move(currentPosition))
{
}

}
