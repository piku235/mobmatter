#include "Cover.h"
#include "CoverAdded.h"
#include "CoverCloseRequested.h"
#include "CoverLiftCurrentPositionChanged.h"
#include "CoverLiftMotionChanged.h"
#include "CoverLiftRequested.h"
#include "CoverLiftTargetPositionChanged.h"
#include "CoverMarkedAsReachable.h"
#include "CoverMarkedAsUnreachable.h"
#include "CoverMotion.h"
#include "CoverNameChanged.h"
#include "CoverOpenRequested.h"
#include "CoverRemoved.h"
#include "CoverRenameRequested.h"
#include "CoverStopMotionRequested.h"
#include "CoverTiltCurrentPositionChanged.h"
#include "CoverTiltMotionChanged.h"
#include "CoverTiltRequested.h"
#include "CoverTiltTargetPositionChanged.h"

using namespace mobmatter::common::domain;

namespace mobmatter::application::model::window_covering {

Cover Cover::add(EndpointId endpointId, MobilusDeviceId mobilusDeviceId, CoverSpecification specification, std::string name, PositionState liftState, PositionState tiltState)
{
    raise(std::make_unique<CoverAdded>(endpointId, mobilusDeviceId, specification));

    return {
        endpointId,
        mobilusDeviceId,
        UniqueId::random(),
        std::move(specification),
        true,
        std::move(name),
        std::move(liftState),
        std::move(tiltState),
    };
}

Cover Cover::restoreFrom(EndpointId endpointId, MobilusDeviceId mobilusDeviceId, UniqueId uniqueId, CoverSpecification specification, bool reachable, std::string name, PositionState liftState, PositionState tiltState)
{
    return {
        endpointId,
        mobilusDeviceId,
        std::move(uniqueId),
        std::move(specification),
        reachable,
        std::move(name),
        std::move(liftState),
        std::move(tiltState),
    };
}

Cover::Cover(EndpointId endpointId, MobilusDeviceId mobilusDeviceId, UniqueId uniqueId, CoverSpecification specification, bool reachable, std::string name, PositionState liftState, PositionState tiltState)
    : mEndpointId(endpointId)
    , mMobilusDeviceId(mobilusDeviceId)
    , mUniqueId(std::move(uniqueId))
    , mSpecification(std::move(specification))
    , mReachable(reachable)
    , mName(std::move(name))
    , mLiftState(std::move(liftState))
    , mTiltState(std::move(tiltState))
{
}

Cover::Result Cover::requestOpen()
{
    return requestLiftAndTiltTo<CoverOpenRequested>(Position::fullyOpen());
}

Cover::Result Cover::requestClose()
{
    return requestLiftAndTiltTo<CoverCloseRequested>(Position::fullyClosed());
}

Cover::Result Cover::requestLiftTo(Position position)
{
    if (PositionStatus::Unavailable == mLiftState.status()) {
        return Result::NotSupported;
    }
    if (position == mLiftState.targetPosition()) {
        return Result::NoChange;
    }

    replaceLiftState(mLiftState.requestMoveTo(position));
    raise(std::make_unique<CoverLiftRequested>(mEndpointId, mMobilusDeviceId, position));

    return Result::Ok;
}

Cover::Result Cover::requestTiltTo(Position position)
{
    if (PositionStatus::Unavailable == mTiltState.status()) {
        return Result::NotSupported;
    }
    if (position == mTiltState.targetPosition()) {
        return Result::NoChange;
    }

    replaceTiltState(mTiltState.requestMoveTo(position));
    raise(std::make_unique<CoverTiltRequested>(mEndpointId, mMobilusDeviceId, position));

    return Result::Ok;
}

Cover::Result Cover::requestStopMotion()
{
    bool liftMovement = PositionStatus::Requested == mLiftState.status() || PositionStatus::Moving == mLiftState.status();
    bool tiltMovement = PositionStatus::Requested == mTiltState.status() || PositionStatus::Moving == mTiltState.status();

    if (!liftMovement && !tiltMovement) {
        return Result::NoChange;
    }

    if (liftMovement) {
        replaceLiftState(mLiftState.stop());
    }
    if (tiltMovement) {
        replaceTiltState(mTiltState.stop());
    }

    raise(std::make_unique<CoverStopMotionRequested>(mEndpointId, mMobilusDeviceId));

    return Result::Ok;
}

Cover::Result Cover::requestRename(std::string name)
{
    if (mName == name) {
        return Result::NoChange;
    }

    mName = std::move(name);

    raise(std::make_unique<CoverNameChanged>(mEndpointId, mMobilusDeviceId, mName));
    raise(std::make_unique<CoverRenameRequested>(mEndpointId, mMobilusDeviceId, mName));

    return Result::Ok;
}

Cover::Result Cover::reportLiftTo(Position position)
{
    if (PositionStatus::Unavailable == mLiftState.status()) {
        return Result::NotSupported;
    }
    if (position == mLiftState.targetPosition() && PositionStatus::Requested != mLiftState.status()) {
        return Result::NoChange;
    }

    replaceLiftState(mLiftState.movingTo(position));

    return Result::Ok;
}

Cover::Result Cover::reportLiftPosition(Position position)
{
    if (PositionStatus::Unavailable == mLiftState.status()) {
        return Result::NotSupported;
    }
    if (position == mLiftState.currentPosition() && position == mLiftState.targetPosition()) {
        return Result::NoChange;
    }

    replaceLiftState(PositionState::at(position));

    return Result::Ok;
}

Cover::Result Cover::reportTiltTo(Position position)
{
    if (PositionStatus::Unavailable == mTiltState.status()) {
        return Result::NotSupported;
    }
    if (position == mTiltState.targetPosition() && PositionStatus::Requested != mTiltState.status()) {
        return Result::NoChange;
    }

    replaceTiltState(mTiltState.movingTo(position));

    return Result::Ok;
}

Cover::Result Cover::reportTiltPosition(Position position)
{
    if (PositionStatus::Unavailable == mTiltState.status()) {
        return Result::NotSupported;
    }
    if (position == mTiltState.currentPosition() && position == mTiltState.targetPosition()) {
        return Result::NoChange;
    }

    replaceTiltState(PositionState::at(position));

    return Result::Ok;
}

Cover::Result Cover::reportStopMotion()
{
    bool liftMovement = PositionStatus::Moving == mLiftState.status();
    bool tiltMovement = PositionStatus::Moving == mTiltState.status();

    if (!liftMovement && !tiltMovement) {
        return Result::NoChange;
    }

    if (liftMovement) {
        replaceLiftState(mLiftState.stop());
    }
    if (tiltMovement) {
        replaceTiltState(mTiltState.stop());
    }

    return Result::Ok;
}

Cover::Result Cover::reportMotionFailure()
{
    bool liftMovement = PositionStatus::Moving == mLiftState.status();
    bool tiltMovement = PositionStatus::Moving == mTiltState.status();

    if (!liftMovement && !tiltMovement) {
        return Result::NoChange;
    }

    if (liftMovement) {
        replaceLiftState(mLiftState.reset());
    }
    if (tiltMovement) {
        replaceTiltState(mTiltState.reset());
    }

    return Result::Ok;
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

Cover::Result Cover::reportUnreachable()
{
    if (!mReachable) {
        return Result::NoChange;
    }

    mReachable = false;
    raise(std::make_unique<CoverMarkedAsUnreachable>(mEndpointId, mMobilusDeviceId));

    return Result::Ok;
}

Cover::Result Cover::reportRenamedTo(std::string name)
{
    if (mName == name) {
        return Result::NoChange;
    }

    mName = std::move(name);
    raise(std::make_unique<CoverNameChanged>(mEndpointId, mMobilusDeviceId, mName));

    return Result::Ok;
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

const UniqueId& Cover::uniqueId() const
{
    return mUniqueId;
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

void Cover::replaceLiftState(PositionState&& liftState)
{
    if (liftState.motion() != mLiftState.motion()) {
        raise(std::make_unique<CoverLiftMotionChanged>(mEndpointId, mMobilusDeviceId, liftState.motion()));
    }

    if (liftState.targetPosition() != mLiftState.targetPosition()) {
        raise(std::make_unique<CoverLiftTargetPositionChanged>(mEndpointId, mMobilusDeviceId, *liftState.targetPosition()));
    }

    if (liftState.currentPosition() != mLiftState.currentPosition()) {
        raise(std::make_unique<CoverLiftCurrentPositionChanged>(mEndpointId, mMobilusDeviceId, *liftState.currentPosition()));
    }

    mLiftState = std::move(liftState);
}

void Cover::replaceTiltState(PositionState&& tiltState)
{
    if (tiltState.motion() != mTiltState.motion()) {
        raise(std::make_unique<CoverTiltMotionChanged>(mEndpointId, mMobilusDeviceId, tiltState.motion()));
    }

    if (tiltState.targetPosition() != mTiltState.targetPosition()) {
        raise(std::make_unique<CoverTiltTargetPositionChanged>(mEndpointId, mMobilusDeviceId, *tiltState.targetPosition()));
    }

    if (tiltState.currentPosition() != mTiltState.currentPosition()) {
        raise(std::make_unique<CoverTiltCurrentPositionChanged>(mEndpointId, mMobilusDeviceId, *tiltState.currentPosition()));
    }

    mTiltState = std::move(tiltState);
}

template <typename Event>
Cover::Result Cover::requestLiftAndTiltTo(Position position)
{
    bool liftPositionDiffers = PositionStatus::Unavailable != mLiftState.status() && position != mLiftState.targetPosition();
    bool tiltPositionDiffers = PositionStatus::Unavailable != mTiltState.status() && position != mTiltState.targetPosition();

    if (!liftPositionDiffers && !tiltPositionDiffers) {
        return Result::NoChange;
    }

    if (liftPositionDiffers) {
        replaceLiftState(mLiftState.requestMoveTo(position));
    }
    if (tiltPositionDiffers) {
        replaceTiltState(mTiltState.requestMoveTo(position));
    }

    raise(std::make_unique<Event>(mEndpointId, mMobilusDeviceId));

    return Result::Ok;
}

}
