#pragma once

#include "CoverMotion.h"
#include "Position.h"
#include "PositionStatus.h"

#include <optional>

namespace mobmatter::application::model::window_covering {

class [[nodiscard]] PositionState final {
public:
    static PositionState at(Position position);
    static PositionState restore(PositionStatus status, CoverMotion motion, Position targetPosition, Position currentPosition);

    [[nodiscard]] std::optional<PositionState> movingTo(Position position) const;
    [[nodiscard]] std::optional<PositionState> stop() const;
    [[nodiscard]] std::optional<PositionState> nowAt(Position position) const;

    PositionStatus status() const { return mStatus; }
    CoverMotion motion() const { return mMotion; }
    [[nodiscard]] Position targetPosition() const { return mTargetPosition; }
    [[nodiscard]] Position currentPosition() const { return mCurrentPosition; }

private:
    /* const */ PositionStatus mStatus;
    /* const */ CoverMotion mMotion;
    /* const */ Position mTargetPosition;
    /* const */ Position mCurrentPosition;

    PositionState(PositionStatus status, CoverMotion motion, Position targetPosition, Position currentPosition);
};

}
