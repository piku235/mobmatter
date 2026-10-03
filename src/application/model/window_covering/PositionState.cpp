#include "PositionState.h"

namespace mobmatter::application::model::window_covering {

PositionState PositionState::at(Position position)
{
    return { PositionStatus::Idle, CoverMotion::NotMoving, position, position };
}

PositionState PositionState::restore(PositionStatus status, CoverMotion motion, Position targetPosition, Position currentPosition)
{
    return { status, motion, targetPosition, currentPosition };
}

std::optional<PositionState> PositionState::movingTo(Position position) const
{
    if (PositionStatus::Moving == mStatus && mTargetPosition == position) {
        return std::nullopt;
    }

    return PositionState {
        PositionStatus::Moving,
        position.isHigherThan(mCurrentPosition) ? CoverMotion::Opening : CoverMotion::Closing,
        position,
        mCurrentPosition,
    };
}

std::optional<PositionState> PositionState::stop() const
{
    if (PositionStatus::Stopping == mStatus) {
        return std::nullopt;
    }

    return PositionState {
        PositionStatus::Stopping,
        mMotion,
        mTargetPosition,
        mCurrentPosition,
    };
}

std::optional<PositionState> PositionState::nowAt(Position position) const
{
    if (PositionStatus::Idle == mStatus && mCurrentPosition == position) {
        return std::nullopt;
    }

    return at(position);
}

PositionState::PositionState(PositionStatus status, CoverMotion motion, Position targetPosition, Position currentPosition)
    : mStatus(status)
    , mMotion(motion)
    , mTargetPosition(targetPosition)
    , mCurrentPosition(currentPosition)
{
}

}
