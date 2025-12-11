#include "Cover.h"
#include "CoverAdded.h"
#include "CoverLiftCurrentPositionChanged.h"
#include "CoverLiftMotionChanged.h"
#include "CoverLiftRequested.h"
#include "CoverLiftTargetPositionChanged.h"
#include "CoverMarkedAsReachable.h"
#include "CoverMarkedAsUnreachable.h"
#include "CoverMotion.h"
#include "CoverNameChanged.h"
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

Cover::Result Cover::reportLiftTo(Position position)
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

Cover::Result Cover::reportLiftPosition(Position position)
{
    if (PositionStatus::Unavailable == mLiftState.status()) {
        return Result::LiftNotSupported;
    }

    auto result = Result::NoChange;

    // this is the only feedback once device becomes reachable again
    if (!mReachable) {
        mReachable = true;
        raise(std::make_unique<CoverMarkedAsReachable>(mEndpointId, mMobilusDeviceId));

        result = Result::Ok;
    }

    if (position != mLiftState.currentPosition() || position != mLiftState.targetPosition()) {
        replaceLiftState(PositionState::at(position));

        result = Result::Ok;
    }

    return result;
}

Cover::Result Cover::reportStopMotion()
{
    if (PositionStatus::Moving == mLiftState.status()) {
        replaceLiftState(mLiftState.stop());
        return Result::Ok;
    }

    return Result::NoChange;
}

Cover::Result Cover::reportMotionFailure()
{
    if (PositionStatus::Moving == mLiftState.status()) {
        replaceLiftState(mLiftState.reset());
        return Result::Ok;
    }

    return Result::NoChange;
}

Cover::Result Cover::reportUnreachable()
{
    Result result = Result::NoChange;

    if (mReachable) {
        mReachable = false;
        raise(std::make_unique<CoverMarkedAsUnreachable>(mEndpointId, mMobilusDeviceId));

        result = Result::Ok;
    }

    if (PositionStatus::Moving == mLiftState.status()) {
        replaceLiftState(mLiftState.reset());

        result = Result::Ok;
    }

    return result;
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

const std::string& Cover::name() const
{
    return mName;
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

}
