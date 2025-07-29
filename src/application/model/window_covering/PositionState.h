#pragma once

#include "CoverMotion.h"
#include "Position.h"
#include "PositionStatus.h"

#include <optional>

namespace mmbridge::application::model::window_covering {

struct PositionState final {
public:
    static PositionState unavailable();
    static PositionState at(Position position);
    static PositionState restore(PositionStatus status, CoverMotion motion, std::optional<Position> targetPosition, std::optional<Position> currentPosition);

    PositionState movingTo(Position position) const;
    PositionState requestMoveTo(Position position) const;
    PositionState stop() const;
    PositionState reset() const;

    PositionStatus status() const { return mStatus; }
    CoverMotion motion() const { return mMotion; }
    std::optional<Position> targetPosition() const { return mTargetPosition; }
    std::optional<Position> currentPosition() const { return mCurrentPosition; }

private:
    /* const */ PositionStatus mStatus;
    /* const */ CoverMotion mMotion;
    /* const */ std::optional<Position> mTargetPosition;
    /* const */ std::optional<Position> mCurrentPosition;

    PositionState(PositionStatus status, CoverMotion motion, std::optional<Position> targetPosition, std::optional<Position> currentPosition);
};

}
