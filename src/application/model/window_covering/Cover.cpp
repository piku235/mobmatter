#include "Cover.h"
#include "CoverEvents.h"

using namespace mobmatter::common::domain;

namespace mobmatter::application::model::window_covering {

Cover Cover::add(EndpointId endpointId, MobilusDeviceId mobilusDeviceId, CoverSpecification specification, std::string name, PositionState liftState, std::optional<PositionState> tiltState)
{
    raise(std::make_unique<CoverAdded>(endpointId, mobilusDeviceId, specification));

    return {
        endpointId,
        mobilusDeviceId,
        std::move(specification),
        true,
        std::move(name),
        std::nullopt,
        liftState,
        tiltState,
    };
}

Cover Cover::restoreFrom(EndpointId endpointId, MobilusDeviceId mobilusDeviceId, CoverSpecification specification, bool reachable, std::string name, std::optional<CoverMotionFault> motionFault, PositionState liftState, std::optional<PositionState> tiltState)
{
    return {
        endpointId,
        mobilusDeviceId,
        std::move(specification),
        reachable,
        std::move(name),
        motionFault,
        liftState,
        tiltState,
    };
}

Cover::Cover(EndpointId endpointId, MobilusDeviceId mobilusDeviceId, CoverSpecification specification, bool reachable, std::string name, std::optional<CoverMotionFault> motionFault, PositionState liftState, std::optional<PositionState> tiltState)
    : Device(endpointId, mobilusDeviceId, std::move(name))
    , mSpecification(std::move(specification))
    , mReachable(reachable)
    , mMotionFault(motionFault)
    , mLiftState(liftState)
    , mTiltState(tiltState)
{
}

Cover::Result Cover::requestOpen()
{
    auto result = changeLiftAndTiltTargetPosition(Position::fullyOpen());

    if (Result::Ok == result) {
        raise(std::make_unique<CoverOpenRequested>(mEndpointId, mMobilusDeviceId));
    }

    return result;
}

Cover::Result Cover::requestClose()
{
    auto result = changeLiftAndTiltTargetPosition(Position::fullyClosed());

    if (Result::Ok == result) {
        raise(std::make_unique<CoverCloseRequested>(mEndpointId, mMobilusDeviceId));
    }

    return result;
}

Cover::Result Cover::requestLiftTo(Position position)
{
    auto result = changeLiftTargetPosition(position);

    if (Result::Ok == result) {
        raise(std::make_unique<CoverLiftRequested>(mEndpointId, mMobilusDeviceId, position));
    }

    return result;
}

Cover::Result Cover::requestTiltTo(Position position)
{
    auto result = changeTiltTargetPosition(position);

    if (Result::Ok == result) {
        raise(std::make_unique<CoverTiltRequested>(mEndpointId, mMobilusDeviceId, position));
    }

    return result;
}

Cover::Result Cover::requestStopMotion()
{
    auto liftState = mLiftState.stop();
    auto tiltState = mTiltState ? mTiltState->stop() : std::nullopt;

    if (!liftState && !tiltState) {
        return Result::NoChange;
    }

    if (liftState) {
        mLiftState = *liftState;
    }
    if (tiltState) {
        mTiltState = tiltState;
    }

    raise(std::make_unique<CoverStopMotionRequested>(mEndpointId, mMobilusDeviceId));
    return Result::Ok;
}

Cover::Result Cover::reportOpen()
{
    return changeLiftAndTiltTargetPosition(Position::fullyOpen());
}

Cover::Result Cover::reportClose()
{
    return changeLiftAndTiltTargetPosition(Position::fullyClosed());
}

Cover::Result Cover::reportLiftTo(Position position)
{
    return changeLiftTargetPosition(position);
}

Cover::Result Cover::reportLiftPosition(Position position)
{
    auto liftState = mLiftState.nowAt(position);
    if (!liftState) {
        return Result::NoChange;
    }

    auto motionChanged = liftState->motion() != mLiftState.motion();
    auto targetPositionChanged = liftState->targetPosition() != mLiftState.targetPosition();
    auto currentPositionChanged = liftState->currentPosition() != mLiftState.currentPosition();

    mLiftState = *liftState;

    if (motionChanged) {
        raise(std::make_unique<CoverLiftMotionChanged>(mEndpointId, mMobilusDeviceId, mLiftState.motion()));
    }
    if (targetPositionChanged) {
        raise(std::make_unique<CoverLiftTargetPositionChanged>(mEndpointId, mMobilusDeviceId, position));
    }
    if (currentPositionChanged) {
        raise(std::make_unique<CoverLiftCurrentPositionChanged>(mEndpointId, mMobilusDeviceId, position));
    }

    return Result::Ok;
}

Cover::Result Cover::reportTiltTo(Position position)
{
    return changeTiltTargetPosition(position);
}

Cover::Result Cover::reportTiltPosition(Position position)
{
    if (!mTiltState) {
        return Result::NotSupported;
    }

    auto tiltState = mTiltState->nowAt(position);
    if (!tiltState) {
        return Result::NoChange;
    }

    auto motionChanged = tiltState->motion() != mTiltState->motion();
    auto targetPositionChanged = tiltState->targetPosition() != mTiltState->targetPosition();
    auto currentPositionChanged = tiltState->currentPosition() != mTiltState->currentPosition();

    mTiltState = *tiltState;

    if (motionChanged) {
        raise(std::make_unique<CoverTiltMotionChanged>(mEndpointId, mMobilusDeviceId, mTiltState->motion()));
    }
    if (targetPositionChanged) {
        raise(std::make_unique<CoverTiltTargetPositionChanged>(mEndpointId, mMobilusDeviceId, position));
    }
    if (currentPositionChanged) {
        raise(std::make_unique<CoverTiltCurrentPositionChanged>(mEndpointId, mMobilusDeviceId, position));
    }

    return Result::Ok;
}

Cover::Result Cover::reportAsReachable()
{
    if (mReachable) {
        return Result::NoChange;
    }

    mReachable = true;
    raise(std::make_unique<CoverBecameReachable>(mEndpointId, mMobilusDeviceId));

    return Result::Ok;
}

Cover::Result Cover::reportAsUnreachable()
{
    if (!mReachable) {
        return Result::NoChange;
    }

    mReachable = false;
    raise(std::make_unique<CoverBecameUnreachable>(mEndpointId, mMobilusDeviceId));

    return Result::Ok;
}

Cover::Result Cover::reportMotionFault(CoverMotionFault fault)
{
    if (mMotionFault == fault) {
        return Result::NoChange;
    }

    auto liftMovement = PositionStatus::Moving == mLiftState.status();
    auto tiltMovement = mTiltState && PositionStatus::Moving == mTiltState->status();

    if (!liftMovement && !tiltMovement) {
        return Result::NoChange;
    }

    mMotionFault = fault;

    if (liftMovement) {
        mLiftState = PositionState::at(mLiftState.currentPosition());
        raise(std::make_unique<CoverLiftMotionChanged>(mEndpointId, mMobilusDeviceId, mLiftState.motion()));
        raise(std::make_unique<CoverLiftTargetPositionChanged>(mEndpointId, mMobilusDeviceId, mLiftState.targetPosition()));
    }
    if (tiltMovement) {
        mTiltState = PositionState::at(mTiltState->currentPosition());
        raise(std::make_unique<CoverTiltMotionChanged>(mEndpointId, mMobilusDeviceId, mTiltState->motion()));
        raise(std::make_unique<CoverTiltTargetPositionChanged>(mEndpointId, mMobilusDeviceId, mTiltState->targetPosition()));
    }

    return Result::Ok;
}

Cover::Result Cover::changeLiftAndTiltTargetPosition(Position position)
{
    auto liftResult = changeLiftTargetPosition(position);
    auto tiltResult = changeTiltTargetPosition(position);

    return Result::Ok == liftResult || Result::Ok == tiltResult ? Result::Ok : Result::NoChange;
}

Cover::Result Cover::changeLiftTargetPosition(Position position)
{
    if (auto state = mLiftState.movingTo(position)) {
        mMotionFault = std::nullopt;
        mLiftState = *state;

        raise(std::make_unique<CoverLiftMotionChanged>(mEndpointId, mMobilusDeviceId, mLiftState.motion()));
        raise(std::make_unique<CoverLiftTargetPositionChanged>(mEndpointId, mMobilusDeviceId, position));

        return Result::Ok;
    }

    return Result::NoChange;
}

Cover::Result Cover::changeTiltTargetPosition(Position position)
{
    if (!mTiltState) {
        return Result::NotSupported;
    }
    if (auto state = mTiltState->movingTo(position)) {
        mMotionFault = std::nullopt;
        mTiltState = state;

        raise(std::make_unique<CoverTiltMotionChanged>(mEndpointId, mMobilusDeviceId, mTiltState->motion()));
        raise(std::make_unique<CoverTiltTargetPositionChanged>(mEndpointId, mMobilusDeviceId, position));

        return Result::Ok;
    }

    return Result::NoChange;
}

std::unique_ptr<DomainEvent> Cover::deviceRemoved()
{
    return std::make_unique<CoverRemoved>(mEndpointId, mMobilusDeviceId);
}

std::unique_ptr<DomainEvent> Cover::deviceRenamed()
{
    return std::make_unique<CoverRenamed>(mEndpointId, mMobilusDeviceId, mName);
}

}
