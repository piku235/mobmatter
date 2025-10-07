#include "Cover.h"
#include "CoverAdded.h"
#include "CoverLiftCurrentPositionChanged.h"
#include "CoverLiftRequested.h"
#include "CoverLiftTargetPositionChanged.h"
#include "CoverMarkedAsReachable.h"
#include "CoverMarkedAsUnreachable.h"
#include "CoverMotion.h"
#include "CoverOperationalStatusChanged.h"
#include "CoverRemoved.h"
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

Cover::Result<> Cover::requestLiftTo(Position position)
{
    if (auto e = assertLiftIsAvailable(); !e) {
        return e;
    }
    if (position == mLiftState.targetPosition()) {
        return {};
    }

    replaceLiftState(mLiftState.requestMoveTo(position));
    raise(std::make_unique<CoverLiftRequested>(mEndpointId, mMobilusDeviceId, position));

    return {};
}

void Cover::requestOpen()
{
    (void)requestLiftTo(Position::fullyOpen());
}

void Cover::requestClose()
{
    (void)requestLiftTo(Position::fullyClosed());
}

Cover::Result<> Cover::startLiftTo(Position position)
{
    if (auto e = assertLiftIsAvailable(); !e) {
        return e;
    }
    if (position == mLiftState.targetPosition() && PositionStatus::Moving == mLiftState.status()) {
        return {};
    }

    replaceLiftState(mLiftState.movingTo(position));
    return {};
}

Cover::Result<> Cover::changeLiftPosition(Position position)
{
    if (auto e = assertLiftIsAvailable(); !e) {
        return e;
    }

    // no other feedback once device becomes reachable again
    markAsReachable();

    if (PositionStatus::Idle == mLiftState.status() && position == mLiftState.currentPosition()) {
        return {};
    }

    replaceLiftState(PositionState::at(position));
    return {};
}

void Cover::requestStopMotion()
{
    if (PositionStatus::Requested == mLiftState.status() || PositionStatus::Moving == mLiftState.status()) {
        replaceLiftState(mLiftState.stop());
        raise(std::make_unique<CoverStopMotionRequested>(mEndpointId, mMobilusDeviceId));
    }
}

void Cover::initiateStopMotion()
{
    if (PositionStatus::Moving == mLiftState.status()) {
        replaceLiftState(mLiftState.stop());
    }
}

void Cover::failMotion()
{
    if (PositionStatus::Moving == mLiftState.status()) {
        replaceLiftState(mLiftState.reset());
    }
}

void Cover::markAsUnreachable()
{
    if (mReachable) {
        mReachable = false;
        raise(std::make_unique<CoverMarkedAsUnreachable>(mEndpointId, mMobilusDeviceId));
    }

    if (PositionStatus::Moving == mLiftState.status()) {
        replaceLiftState(mLiftState.reset());
    }
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

Cover::Result<> Cover::assertLiftIsAvailable()
{
    if (PositionStatus::Unavailable == mLiftState.status()) {
        return tl::unexpected(DomainError(ErrorCode::LiftUnavailable, "Lift is not available for this cover"));
    }

    return {};
}

}
