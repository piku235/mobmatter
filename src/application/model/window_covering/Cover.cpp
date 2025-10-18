#include "Cover.h"
#include "CoverAdded.h"
#include "CoverLiftCurrentPositionChanged.h"
#include "CoverLiftRequested.h"
#include "CoverLiftTargetPositionChanged.h"
#include "CoverMarkedAsReachable.h"
#include "CoverMarkedAsUnreachable.h"
#include "CoverMotion.h"
#include "CoverNameChanged.h"
#include "CoverOperationalStatusChanged.h"
#include "CoverRemoved.h"
#include "CoverRenameRequested.h"
#include "CoverStopMotionRequested.h"

using namespace mobmatter::common::domain;

namespace mobmatter::application::model::window_covering {

Cover Cover::add(EndpointId endpointId, MobilusDeviceId mobilusDeviceId, std::string name, PositionState liftState, CoverSpecification specification)
{
    raise(std::make_unique<CoverAdded>(endpointId, mobilusDeviceId, specification));

    return {
        endpointId,
        mobilusDeviceId,
        UniqueId::random(),
        true,
        std::move(name),
        std::move(liftState),
        std::move(specification),
    };
}

Cover Cover::restoreFrom(EndpointId endpointId, MobilusDeviceId mobilusDeviceId, UniqueId uniqueId, bool reachable, std::string name, PositionState liftState, CoverSpecification specification)
{
    return {
        endpointId,
        mobilusDeviceId,
        std::move(uniqueId),
        reachable,
        std::move(name),
        std::move(liftState),
        std::move(specification),
    };
}

Cover::Cover(EndpointId endpointId, MobilusDeviceId mobilusDeviceId, UniqueId uniqueId, bool reachable, std::string name, PositionState liftState, CoverSpecification specification)
    : mEndpointId(endpointId)
    , mMobilusDeviceId(mobilusDeviceId)
    , mUniqueId(std::move(uniqueId))
    , mSpecification(std::move(specification))
    , mReachable(reachable)
    , mName(std::move(name))
    , mLiftState(std::move(liftState))
{
}

Cover::Result Cover::requestLiftTo(Position position)
{
    if (PositionStatus::Unavailable == mLiftState.status()) {
        return Result::LiftNotSupported;
    }
    if (position == mLiftState.targetPosition()) {
        return Result::NoChange;
    }

    replaceLiftState(mLiftState.requestMoveTo(position));
    raise(std::make_unique<CoverLiftRequested>(mEndpointId, mMobilusDeviceId, position));

    return Result::Ok;
}

Cover::Result Cover::requestOpen()
{
    return requestLiftTo(Position::fullyOpen());
}

Cover::Result Cover::requestClose()
{
    return requestLiftTo(Position::fullyClosed());
}

Cover::Result Cover::startLiftTo(Position position)
{
    if (PositionStatus::Unavailable == mLiftState.status()) {
        return Result::LiftNotSupported;
    }
    if (position == mLiftState.targetPosition() && PositionStatus::Moving == mLiftState.status()) {
        return Result::NoChange;
    }

    replaceLiftState(mLiftState.movingTo(position));
    return Result::Ok;
}

Cover::Result Cover::changeLiftPosition(Position position)
{
    if (PositionStatus::Unavailable == mLiftState.status()) {
        return Result::LiftNotSupported;
    }

    // no other feedback once device becomes reachable again
    markAsReachable();

    if (PositionStatus::Idle == mLiftState.status() && position == mLiftState.currentPosition()) {
        return Result::NoChange;
    }

    replaceLiftState(PositionState::at(position));
    return Result::Ok;
}

Cover::Result Cover::requestStopMotion()
{
    if (PositionStatus::Requested == mLiftState.status() || PositionStatus::Moving == mLiftState.status()) {
        replaceLiftState(mLiftState.stop());
        raise(std::make_unique<CoverStopMotionRequested>(mEndpointId, mMobilusDeviceId));

        return Result::Ok;
    }

    return Result::NoChange;
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

Cover::Result Cover::initiateStopMotion()
{
    if (PositionStatus::Moving == mLiftState.status()) {
        replaceLiftState(mLiftState.stop());
        return Result::Ok;
    }

    return Result::NoChange;
}

Cover::Result Cover::failMotion()
{
    if (PositionStatus::Moving == mLiftState.status()) {
        replaceLiftState(mLiftState.reset());
        return Result::Ok;
    }

    return Result::NoChange;
}

Cover::Result Cover::markAsUnreachable()
{
    bool changed = false;

    if (mReachable) {
        mReachable = false;
        raise(std::make_unique<CoverMarkedAsUnreachable>(mEndpointId, mMobilusDeviceId));

        changed = true;
    }

    if (PositionStatus::Moving == mLiftState.status()) {
        replaceLiftState(mLiftState.reset());
        changed = true;
    }

    return changed ? Result::Ok : Result::NoChange;
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

void Cover::remove()
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

const std::string& Cover::name() const
{
    return mName;
}

CoverOperationalStatus Cover::operationalStatus() const
{
    return { mLiftState.motion(), CoverMotion::NotMoving };
}

const PositionState& Cover::liftState() const
{
    return mLiftState;
}

const CoverSpecification& Cover::specification() const
{
    return mSpecification;
}

void Cover::replaceLiftState(PositionState&& liftState)
{
    if (liftState.motion() != mLiftState.motion()) {
        raise(std::make_unique<CoverOperationalStatusChanged>(mEndpointId, mMobilusDeviceId, CoverOperationalStatus(liftState.motion(), CoverMotion::NotMoving)));
    }

    if (liftState.targetPosition() != mLiftState.targetPosition()) {
        raise(std::make_unique<CoverLiftTargetPositionChanged>(mEndpointId, mMobilusDeviceId, *liftState.targetPosition()));
    }

    if (liftState.currentPosition() != mLiftState.currentPosition()) {
        raise(std::make_unique<CoverLiftCurrentPositionChanged>(mEndpointId, mMobilusDeviceId, *liftState.currentPosition()));
    }

    mLiftState = std::move(liftState);
}

void Cover::markAsReachable()
{
    if (mReachable) {
        return;
    }

    mReachable = true;
    raise(std::make_unique<CoverMarkedAsReachable>(mEndpointId, mMobilusDeviceId));
}

}
