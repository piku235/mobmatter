#include "Cover.h"
#include "CoverEvents.h"

using namespace mobmatter::common::domain;

namespace mobmatter::application::model::window_covering {

Cover Cover::add(EndpointId endpointId, MobilusDeviceId mobilusDeviceId, CoverSpecification specification, std::string name, PositionState liftState, PositionState tiltState)
{
    raise(std::make_unique<CoverAdded>(endpointId, mobilusDeviceId, specification));

    return {
        endpointId,
        mobilusDeviceId,
        std::move(specification),
        true,
        std::move(name),
        std::move(liftState),
        std::move(tiltState),
    };
}

Cover Cover::restoreFrom(EndpointId endpointId, MobilusDeviceId mobilusDeviceId, CoverSpecification specification, bool reachable, std::string name, PositionState liftState, PositionState tiltState)
{
    return {
        endpointId,
        mobilusDeviceId,
        std::move(specification),
        reachable,
        std::move(name),
        std::move(liftState),
        std::move(tiltState),
    };
}

Cover::Cover(EndpointId endpointId, MobilusDeviceId mobilusDeviceId, CoverSpecification specification, bool reachable, std::string name, PositionState liftState, PositionState tiltState)
    : mEndpointId(endpointId)
    , mMobilusDeviceId(mobilusDeviceId)
    , mSpecification(std::move(specification))
    , mReachable(reachable)
    , mName(std::move(name))
    , mLiftState(std::move(liftState))
    , mTiltState(std::move(tiltState))
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
    auto result = stopMotion();

    if (Result::Ok == result) {
        raise(std::make_unique<CoverStopMotionRequested>(mEndpointId, mMobilusDeviceId));
    }

    return result;
}

Cover::Result Cover::requestRename(std::string name)
{
    auto result = rename(std::move(name));

    if (Result::Ok == result) {
        raise(std::make_unique<CoverRenameRequested>(mEndpointId, mMobilusDeviceId, mName));
    }

    return result;
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
    if (PositionStatus::Unavailable == mLiftState.status()) {
        return Result::NotSupported;
    }

    bool movement = PositionStatus::Moving == mLiftState.status();
    bool currentPositionChanged = position != mLiftState.currentPosition();
    bool targetPositionChanged = position != mLiftState.targetPosition();

    if (!currentPositionChanged && !targetPositionChanged) {
        return Result::NoChange;
    }

    mLiftState = PositionState::at(position);

    if (movement) {
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
    if (PositionStatus::Unavailable == mTiltState.status()) {
        return Result::NotSupported;
    }

    bool movement = PositionStatus::Moving == mTiltState.status();
    bool currentPositionChanged = position != mTiltState.currentPosition();
    bool targetPositionChanged = position != mTiltState.targetPosition();

    if (!currentPositionChanged && !targetPositionChanged) {
        return Result::NoChange;
    }

    mTiltState = PositionState::at(position);

    if (movement) {
        raise(std::make_unique<CoverTiltMotionChanged>(mEndpointId, mMobilusDeviceId, mTiltState.motion()));
    }
    if (targetPositionChanged) {
        raise(std::make_unique<CoverTiltTargetPositionChanged>(mEndpointId, mMobilusDeviceId, position));
    }
    if (currentPositionChanged) {
        raise(std::make_unique<CoverTiltCurrentPositionChanged>(mEndpointId, mMobilusDeviceId, position));
    }

    return Result::Ok;
}

Cover::Result Cover::reportStopMotion()
{
    return stopMotion();
}

Cover::Result Cover::reportReachable()
{
    if (mReachable) {
        return Result::NoChange;
    }

    mReachable = true;
    raise(std::make_unique<CoverMarkedAsReachable>(mEndpointId, mMobilusDeviceId));

    return Result::Ok;
}

Cover::Result Cover::reportError(Error error)
{
    auto result = Result::NoChange;

    if (Error::Unreachable == error && mReachable) {
        mReachable = false;
        raise(std::make_unique<CoverMarkedAsUnreachable>(mEndpointId, mMobilusDeviceId));

        result = Result::Ok;
    }
    if (PositionStatus::Moving == mLiftState.status()) {
        mLiftState = mLiftState.reset();
        raise(std::make_unique<CoverLiftMotionChanged>(mEndpointId, mMobilusDeviceId, mLiftState.motion()));
        raise(std::make_unique<CoverLiftTargetPositionChanged>(mEndpointId, mMobilusDeviceId, *mLiftState.targetPosition()));

        result = Result::Ok;
    }
    if (PositionStatus::Moving == mTiltState.status()) {
        mTiltState = mTiltState.reset();
        raise(std::make_unique<CoverTiltMotionChanged>(mEndpointId, mMobilusDeviceId, mTiltState.motion()));
        raise(std::make_unique<CoverTiltTargetPositionChanged>(mEndpointId, mMobilusDeviceId, *mTiltState.targetPosition()));

        result = Result::Ok;
    }

    return result;
}

Cover::Result Cover::reportRenamedTo(std::string name)
{
    return rename(std::move(name));
}

void Cover::reportRemoved()
{
    raise(std::make_unique<CoverRemoved>(mEndpointId, mMobilusDeviceId));
}

bool Cover::operator==(const Cover& other) const
{
    return mEndpointId == other.mEndpointId;
}

bool Cover::isReachable() const
{
    return mReachable;
}

EndpointId Cover::endpointId() const
{
    return mEndpointId;
}

MobilusDeviceId Cover::mobilusDeviceId() const
{
    return mMobilusDeviceId;
}

const CoverSpecification& Cover::specification() const
{
    return mSpecification;
}

const std::string& Cover::name() const
{
    return mName;
}

const PositionState& Cover::liftState() const
{
    return mLiftState;
}

const PositionState& Cover::tiltState() const
{
    return mTiltState;
}

Cover::Result Cover::rename(std::string name)
{
    if (mName == name) {
        return Result::NoChange;
    }

    mName = std::move(name);
    raise(std::make_unique<CoverNameChanged>(mEndpointId, mMobilusDeviceId, mName));

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
    if (PositionStatus::Unavailable == mLiftState.status()) {
        return Result::NotSupported;
    }
    if (position == mLiftState.targetPosition()) {
        return Result::NoChange;
    }

    mLiftState = mLiftState.movingTo(position);

    raise(std::make_unique<CoverLiftMotionChanged>(mEndpointId, mMobilusDeviceId, mLiftState.motion()));
    raise(std::make_unique<CoverLiftTargetPositionChanged>(mEndpointId, mMobilusDeviceId, position));

    return Result::Ok;
}

Cover::Result Cover::changeTiltTargetPosition(Position position)
{
    if (PositionStatus::Unavailable == mTiltState.status()) {
        return Result::NotSupported;
    }
    if (position == mTiltState.targetPosition()) {
        return Result::NoChange;
    }

    mTiltState = mTiltState.movingTo(position);

    raise(std::make_unique<CoverTiltMotionChanged>(mEndpointId, mMobilusDeviceId, mTiltState.motion()));
    raise(std::make_unique<CoverTiltTargetPositionChanged>(mEndpointId, mMobilusDeviceId, position));

    return Result::Ok;
}

Cover::Result Cover::stopMotion()
{
    bool liftMovement = PositionStatus::Moving == mLiftState.status();
    bool tiltMovement = PositionStatus::Moving == mTiltState.status();

    if (!liftMovement && !tiltMovement) {
        return Result::NoChange;
    }

    if (liftMovement) {
        mLiftState = mLiftState.stop();
        raise(std::make_unique<CoverLiftMotionChanged>(mEndpointId, mMobilusDeviceId, mLiftState.motion()));
    }
    if (tiltMovement) {
        mTiltState = mTiltState.stop();
        raise(std::make_unique<CoverTiltMotionChanged>(mEndpointId, mMobilusDeviceId, mTiltState.motion()));
    }

    return Result::Ok;
}

}
