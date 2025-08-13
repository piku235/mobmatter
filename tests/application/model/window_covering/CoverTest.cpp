#include "application/model/window_covering/Cover.h"
#include "application/model/window_covering/CoverAdded.h"
#include "application/model/window_covering/CoverLiftCurrentPositionChanged.h"
#include "application/model/window_covering/CoverLiftRequested.h"
#include "application/model/window_covering/CoverLiftTargetPositionChanged.h"
#include "application/model/window_covering/CoverMarkedAsReachable.h"
#include "application/model/window_covering/CoverMarkedAsUnreachable.h"
#include "application/model/window_covering/CoverOperationalStatusChanged.h"
#include "application/model/window_covering/CoverRemoved.h"
#include "application/model/window_covering/CoverStopMotionRequested.h"
#include "common/domain/DomainEventQueue.h"

#include <optional>

#include <gtest/gtest.h>

using namespace mmbridge::common::domain;
using namespace mmbridge::application::model;
using namespace mmbridge::application::model::window_covering;

namespace {

auto coverStub(Position position = Position::fullyOpen())
{
    return Cover::add(1, 2, "foo", PositionState::at(position), CoverSpecification::Senso());
}

auto nonLiftCoverStub()
{
    return Cover::add(10, 20, "baz", PositionState::unavailable(), CoverSpecification::TestCzr());
}

}

TEST(CoverTest, AddsNew)
{
    auto cover = coverStub();
    auto& events = DomainEventQueue::instance();

    EXPECT_EQ(1, cover.endpointId());
    EXPECT_EQ(2, cover.mobilusDeviceId());
    EXPECT_TRUE(cover.isReachable());
    EXPECT_EQ("foo", cover.name());
    EXPECT_EQ(PositionStatus::Idle, cover.liftState().status());
    EXPECT_EQ(CoverMotion::NotMoving, cover.liftState().motion());
    EXPECT_EQ(Position::fullyOpen(), *cover.liftState().targetPosition());
    EXPECT_EQ(Position::fullyOpen(), *cover.liftState().currentPosition());
    EXPECT_EQ(CoverSpecification::Senso(), cover.specification());

    EXPECT_EQ(1u, events.size());
    EXPECT_STREQ(CoverAdded::kEventName, events.peek()->eventName());

    auto& event = static_cast<const CoverAdded&>(*events.peek());

    EXPECT_EQ(cover.endpointId(), event.endpointId);
    EXPECT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
    EXPECT_EQ(cover.specification(), event.specification);

    (void)events.pop();
}

TEST(CoverTest, Restores)
{
    auto cover = Cover::restoreFrom(1, 2, UniqueId::of("7bc1ac82347f4f64970db8228ed24290"), false, "foo", PositionState::at(Position::fullyClosed()), CoverSpecification::Cmr());
    auto& events = DomainEventQueue::instance();

    EXPECT_EQ(1, cover.endpointId());
    EXPECT_EQ(2, cover.mobilusDeviceId());
    EXPECT_EQ("7bc1ac82347f4f64970db8228ed24290", cover.uniqueId().value());
    EXPECT_FALSE(cover.isReachable());
    EXPECT_EQ("foo", cover.name());
    EXPECT_EQ(PositionStatus::Idle, cover.liftState().status());
    EXPECT_EQ(CoverMotion::NotMoving, cover.liftState().motion());
    EXPECT_EQ(Position::fullyClosed(), *cover.liftState().targetPosition());
    EXPECT_EQ(Position::fullyClosed(), *cover.liftState().currentPosition());
    EXPECT_EQ(CoverSpecification::Cmr(), cover.specification());
    EXPECT_EQ(0u, events.size());
}

TEST(CoverTest, EqualsAndDoesNotEqual)
{
    auto cover = coverStub();
    auto other = nonLiftCoverStub();

    EXPECT_EQ(cover, coverStub(Position::fullyClosed()));
    EXPECT_FALSE(cover == other);
}

TEST(CoverTest, RequestsLiftToPosition)
{
    auto cover = coverStub();
    auto& events = DomainEventQueue::instance();
    events.clear();

    auto e = cover.requestLiftTo(Position::fullyClosed());

    EXPECT_TRUE(e.has_value());
    EXPECT_EQ(PositionStatus::Requested, cover.liftState().status());
    EXPECT_EQ(CoverMotion::NotMoving, cover.liftState().motion());
    EXPECT_EQ(Position::fullyClosed(), cover.liftState().targetPosition());
    EXPECT_EQ(Position::fullyOpen(), cover.liftState().currentPosition());

    EXPECT_EQ(2u, events.size());
    EXPECT_STREQ(CoverLiftTargetPositionChanged::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverLiftTargetPositionChanged&>(*events.peek());

        EXPECT_EQ(cover.endpointId(), event.endpointId);
        EXPECT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        EXPECT_EQ(cover.liftState().targetPosition(), event.position);
    }

    (void)events.pop();
    EXPECT_STREQ(CoverLiftRequested::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverLiftRequested&>(*events.peek());

        EXPECT_EQ(cover.endpointId(), event.endpointId);
        EXPECT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        EXPECT_EQ(cover.liftState().targetPosition(), event.position);
    }

    (void)events.pop();

    auto otherR = cover.requestLiftTo(Position::fullyClosed());

    EXPECT_TRUE(otherR.has_value());
    EXPECT_EQ(0u, events.size());
}

TEST(CoverTest, RequestsLiftToPositionFailsForNonLiftCover)
{
    auto cover = nonLiftCoverStub();
    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.requestLiftTo(Position::fullyClosed());

    EXPECT_FALSE(r.has_value());
    EXPECT_EQ(Cover::ErrorCode::LiftUnavailable, r.error().code());
    EXPECT_EQ(PositionStatus::Unavailable, cover.liftState().status());
    EXPECT_EQ(0u, events.size());
}

TEST(CoverTest, RequestsOpen)
{
    auto cover = coverStub(Position::fullyClosed());
    auto& events = DomainEventQueue::instance();
    events.clear();

    cover.requestOpen();

    EXPECT_EQ(PositionStatus::Requested, cover.liftState().status());
    EXPECT_EQ(CoverMotion::NotMoving, cover.liftState().motion());
    EXPECT_EQ(Position::fullyOpen(), cover.liftState().targetPosition());
    EXPECT_EQ(Position::fullyClosed(), cover.liftState().currentPosition());

    EXPECT_EQ(2u, events.size());
    EXPECT_STREQ(CoverLiftTargetPositionChanged::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverLiftTargetPositionChanged&>(*events.peek());

        EXPECT_EQ(cover.endpointId(), event.endpointId);
        EXPECT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        EXPECT_EQ(cover.liftState().targetPosition(), event.position);
    }

    (void)events.pop();
    EXPECT_STREQ(CoverLiftRequested::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverLiftRequested&>(*events.peek());

        EXPECT_EQ(cover.endpointId(), event.endpointId);
        EXPECT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        EXPECT_EQ(cover.liftState().targetPosition(), event.position);
    }

    (void)events.pop();

    cover.requestOpen();
    EXPECT_EQ(0u, events.size());
}

TEST(CoverTest, RequestsClose)
{
    auto cover = coverStub();
    auto& events = DomainEventQueue::instance();
    events.clear();

    cover.requestClose();

    EXPECT_EQ(PositionStatus::Requested, cover.liftState().status());
    EXPECT_EQ(CoverMotion::NotMoving, cover.liftState().motion());
    EXPECT_EQ(Position::fullyClosed(), cover.liftState().targetPosition());
    EXPECT_EQ(Position::fullyOpen(), cover.liftState().currentPosition());

    EXPECT_EQ(2u, events.size());
    EXPECT_STREQ(CoverLiftTargetPositionChanged::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverLiftTargetPositionChanged&>(*events.peek());

        EXPECT_EQ(cover.endpointId(), event.endpointId);
        EXPECT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        EXPECT_EQ(cover.liftState().targetPosition(), event.position);
    }

    (void)events.pop();
    EXPECT_STREQ(CoverLiftRequested::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverLiftRequested&>(*events.peek());

        EXPECT_EQ(cover.endpointId(), event.endpointId);
        EXPECT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        EXPECT_EQ(cover.liftState().targetPosition(), event.position);
    }

    (void)events.pop();

    cover.requestClose();
    EXPECT_EQ(0u, events.size());
}

TEST(CoverTest, RequestsStopMotion)
{
    auto cover = coverStub();
    EXPECT_TRUE(cover.startLiftTo(Position::fullyClosed()));

    auto& events = DomainEventQueue::instance();
    events.clear();

    cover.requestStopMotion();

    EXPECT_EQ(PositionStatus::Stopping, cover.liftState().status());
    EXPECT_EQ(CoverMotion::Closing, cover.liftState().motion());
    EXPECT_EQ(Position::fullyClosed(), cover.liftState().targetPosition());
    EXPECT_EQ(Position::fullyOpen(), cover.liftState().currentPosition());

    EXPECT_EQ(1u, events.size());
    EXPECT_STREQ(CoverStopMotionRequested::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverStopMotionRequested&>(*events.peek());

        EXPECT_EQ(cover.endpointId(), event.endpointId);
        EXPECT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
    }

    (void)events.pop();

    cover.requestStopMotion();
    EXPECT_EQ(0u, events.size());
}

TEST(CoverTest, RequestsStopMotionDoesNothingIfNotInMotion)
{
    auto cover = coverStub();
    auto& events = DomainEventQueue::instance();
    events.clear();

    cover.requestStopMotion();

    EXPECT_EQ(PositionStatus::Idle, cover.liftState().status());
    EXPECT_EQ(CoverMotion::NotMoving, cover.liftState().motion());
    EXPECT_EQ(0u, events.size());
}

TEST(CoverTest, RequestStopMotionForRequestedLiftResetsLiftState)
{
    auto cover = coverStub();
    cover.requestClose();

    auto& events = DomainEventQueue::instance();
    events.clear();

    cover.requestStopMotion();

    EXPECT_EQ(PositionStatus::Idle, cover.liftState().status());
    EXPECT_EQ(CoverMotion::NotMoving, cover.liftState().motion());
    EXPECT_EQ(Position::fullyOpen(), cover.liftState().targetPosition());
    EXPECT_EQ(Position::fullyOpen(), cover.liftState().currentPosition());

    EXPECT_EQ(1u, events.size());
    EXPECT_STREQ(CoverLiftTargetPositionChanged::kEventName, events.peek()->eventName());

    auto& event = static_cast<const CoverLiftTargetPositionChanged&>(*events.peek());

    EXPECT_EQ(cover.endpointId(), event.endpointId);
    EXPECT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
    EXPECT_EQ(cover.liftState().targetPosition(), event.position);

    (void)events.pop();
}

TEST(CoverTest, InitiatesStopMotion)
{
    auto cover = coverStub();
    EXPECT_TRUE(cover.startLiftTo(Position::fullyClosed()));

    auto& events = DomainEventQueue::instance();
    events.clear();

    cover.initiateStopMotion();

    EXPECT_EQ(PositionStatus::Stopping, cover.liftState().status());
    EXPECT_EQ(CoverMotion::Closing, cover.liftState().motion());
    EXPECT_EQ(Position::fullyClosed(), cover.liftState().targetPosition());
    EXPECT_EQ(Position::fullyOpen(), cover.liftState().currentPosition());
    EXPECT_EQ(0u, events.size());
}

TEST(CoverTest, StartsLiftToClosePosition)
{
    auto cover = coverStub();

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.startLiftTo(Position::fullyClosed());

    EXPECT_TRUE(r.has_value());
    EXPECT_EQ(CoverMotion::Closing, cover.operationalStatus().lift());
    EXPECT_EQ(CoverMotion::NotMoving, cover.operationalStatus().tilt());
    EXPECT_EQ(PositionStatus::Moving, cover.liftState().status());
    EXPECT_EQ(CoverMotion::Closing, cover.liftState().motion());
    EXPECT_EQ(Position::fullyClosed(), cover.liftState().targetPosition());
    EXPECT_EQ(Position::fullyOpen(), cover.liftState().currentPosition());

    EXPECT_EQ(2u, events.size());
    EXPECT_STREQ(CoverOperationalStatusChanged::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverOperationalStatusChanged&>(*events.peek());

        EXPECT_EQ(cover.endpointId(), event.endpointId);
        EXPECT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        EXPECT_EQ(cover.operationalStatus(), event.operationalStatus);
    }

    (void)events.pop();
    EXPECT_STREQ(CoverLiftTargetPositionChanged::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverLiftTargetPositionChanged&>(*events.peek());

        EXPECT_EQ(cover.endpointId(), event.endpointId);
        EXPECT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        EXPECT_EQ(cover.liftState().targetPosition(), event.position);
    }

    (void)events.pop();

    auto otherR = cover.startLiftTo(*cover.liftState().targetPosition());

    EXPECT_TRUE(r.has_value());
    EXPECT_EQ(0u, events.size());
}

TEST(CoverTest, StartsLiftToOpenPosition)
{
    auto cover = coverStub(Position::fullyClosed());

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.startLiftTo(Position::fullyOpen());

    EXPECT_TRUE(r.has_value());
    EXPECT_EQ(CoverMotion::Opening, cover.operationalStatus().lift());
    EXPECT_EQ(CoverMotion::NotMoving, cover.operationalStatus().tilt());
    EXPECT_EQ(PositionStatus::Moving, cover.liftState().status());
    EXPECT_EQ(CoverMotion::Opening, cover.liftState().motion());
    EXPECT_EQ(Position::fullyOpen(), cover.liftState().targetPosition());
    EXPECT_EQ(Position::fullyClosed(), cover.liftState().currentPosition());

    EXPECT_EQ(2u, events.size());
    EXPECT_STREQ(CoverOperationalStatusChanged::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverOperationalStatusChanged&>(*events.peek());

        EXPECT_EQ(cover.endpointId(), event.endpointId);
        EXPECT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        EXPECT_EQ(cover.operationalStatus(), event.operationalStatus);
    }

    (void)events.pop();
    EXPECT_STREQ(CoverLiftTargetPositionChanged::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverLiftTargetPositionChanged&>(*events.peek());

        EXPECT_EQ(cover.endpointId(), event.endpointId);
        EXPECT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        EXPECT_EQ(cover.liftState().targetPosition(), event.position);
    }

    (void)events.pop();

    auto otherR = cover.startLiftTo(*cover.liftState().targetPosition());

    EXPECT_TRUE(r.has_value());
    EXPECT_EQ(0u, events.size());
}

TEST(CoverTest, StartsLiftToPositionAfterRequest)
{
    auto cover = coverStub();
    cover.requestClose();

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.startLiftTo(Position::fullyClosed());

    EXPECT_TRUE(r.has_value());
    EXPECT_EQ(CoverMotion::Closing, cover.operationalStatus().lift());
    EXPECT_EQ(CoverMotion::NotMoving, cover.operationalStatus().tilt());
    EXPECT_EQ(PositionStatus::Moving, cover.liftState().status());
    EXPECT_EQ(CoverMotion::Closing, cover.liftState().motion());
    EXPECT_EQ(Position::fullyClosed(), cover.liftState().targetPosition());
    EXPECT_EQ(Position::fullyOpen(), cover.liftState().currentPosition());

    EXPECT_EQ(1u, events.size());
    EXPECT_STREQ(CoverOperationalStatusChanged::kEventName, events.peek()->eventName());

    auto& event = static_cast<const CoverOperationalStatusChanged&>(*events.peek());

    EXPECT_EQ(cover.endpointId(), event.endpointId);
    EXPECT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
    EXPECT_EQ(cover.operationalStatus(), event.operationalStatus);

    (void)events.pop();
}

TEST(CoverTest, StartLiftToPositionFailsForNonLiftCover)
{
    auto cover = nonLiftCoverStub();

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.startLiftTo(Position::fullyClosed());

    EXPECT_FALSE(r.has_value());
    EXPECT_EQ(Cover::ErrorCode::LiftUnavailable, r.error().code());
    EXPECT_EQ(PositionStatus::Unavailable, cover.liftState().status());
    EXPECT_EQ(0u, events.size());
}

TEST(CoverTest, SyncesLiftPosition)
{
    auto cover = coverStub();
    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.changeLiftPosition(Position::fullyClosed());

    EXPECT_TRUE(r.has_value());
    EXPECT_EQ(CoverMotion::NotMoving, cover.operationalStatus().lift());
    EXPECT_EQ(CoverMotion::NotMoving, cover.operationalStatus().tilt());
    EXPECT_EQ(PositionStatus::Idle, cover.liftState().status());
    EXPECT_EQ(CoverMotion::NotMoving, cover.liftState().motion());
    EXPECT_EQ(Position::fullyClosed(), cover.liftState().targetPosition());
    EXPECT_EQ(Position::fullyClosed(), cover.liftState().currentPosition());

    EXPECT_EQ(2u, events.size());
    EXPECT_STREQ(CoverLiftTargetPositionChanged::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverLiftTargetPositionChanged&>(*events.peek());

        EXPECT_EQ(cover.endpointId(), event.endpointId);
        EXPECT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        EXPECT_EQ(*cover.liftState().targetPosition(), event.position);
    }

    (void)events.pop();
    EXPECT_STREQ(CoverLiftCurrentPositionChanged::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverLiftCurrentPositionChanged&>(*events.peek());

        EXPECT_EQ(cover.endpointId(), event.endpointId);
        EXPECT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        EXPECT_EQ(*cover.liftState().currentPosition(), event.position);
    }

    (void)events.pop();

    auto otherR = cover.changeLiftPosition(*cover.liftState().currentPosition());
    EXPECT_TRUE(otherR.has_value());
    EXPECT_EQ(0u, events.size());
}

TEST(CoverTest, ChangesLiftPositionAfterStartedMoving)
{
    auto cover = coverStub();
    EXPECT_TRUE(cover.startLiftTo(Position::fullyClosed()));

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.changeLiftPosition(Position::fullyClosed());

    EXPECT_TRUE(r.has_value());
    EXPECT_EQ(CoverMotion::NotMoving, cover.operationalStatus().lift());
    EXPECT_EQ(CoverMotion::NotMoving, cover.operationalStatus().tilt());
    EXPECT_EQ(PositionStatus::Idle, cover.liftState().status());
    EXPECT_EQ(CoverMotion::NotMoving, cover.liftState().motion());
    EXPECT_EQ(Position::fullyClosed(), cover.liftState().targetPosition());
    EXPECT_EQ(Position::fullyClosed(), cover.liftState().currentPosition());

    EXPECT_EQ(2u, events.size());
    EXPECT_STREQ(CoverOperationalStatusChanged::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverOperationalStatusChanged&>(*events.peek());

        EXPECT_EQ(cover.endpointId(), event.endpointId);
        EXPECT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        EXPECT_EQ(cover.operationalStatus(), event.operationalStatus);
    }

    (void)events.pop();
    EXPECT_STREQ(CoverLiftCurrentPositionChanged::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverLiftCurrentPositionChanged&>(*events.peek());

        EXPECT_EQ(cover.endpointId(), event.endpointId);
        EXPECT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        EXPECT_EQ(*cover.liftState().currentPosition(), event.position);
    }

    (void)events.pop();
}

TEST(CoverTest, ChangesLiftPositionAfterMoveRequest)
{
    auto cover = coverStub();
    EXPECT_TRUE(cover.requestLiftTo(Position::fullyClosed()));

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.changeLiftPosition(Position::fullyClosed());

    EXPECT_TRUE(r.has_value());
    EXPECT_EQ(CoverMotion::NotMoving, cover.operationalStatus().lift());
    EXPECT_EQ(CoverMotion::NotMoving, cover.operationalStatus().tilt());
    EXPECT_EQ(PositionStatus::Idle, cover.liftState().status());
    EXPECT_EQ(CoverMotion::NotMoving, cover.liftState().motion());
    EXPECT_EQ(Position::fullyClosed(), cover.liftState().targetPosition());
    EXPECT_EQ(Position::fullyClosed(), cover.liftState().currentPosition());

    EXPECT_EQ(1u, events.size());
    EXPECT_STREQ(CoverLiftCurrentPositionChanged::kEventName, events.peek()->eventName());

    auto& event = static_cast<const CoverLiftCurrentPositionChanged&>(*events.peek());

    EXPECT_EQ(cover.endpointId(), event.endpointId);
    EXPECT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
    EXPECT_EQ(*cover.liftState().currentPosition(), event.position);

    (void)events.pop();
}

TEST(CoverTest, MarksAsUnreachable)
{
    auto cover = coverStub();
    EXPECT_TRUE(cover.startLiftTo(Position::fullyClosed()));

    auto& events = DomainEventQueue::instance();
    events.clear();

    cover.markAsUnreachable();

    EXPECT_FALSE(cover.isReachable());
    EXPECT_EQ(CoverMotion::NotMoving, cover.operationalStatus().lift());
    EXPECT_EQ(CoverMotion::NotMoving, cover.operationalStatus().tilt());
    EXPECT_EQ(PositionStatus::Idle, cover.liftState().status());
    EXPECT_EQ(CoverMotion::NotMoving, cover.liftState().motion());
    EXPECT_EQ(Position::fullyOpen(), cover.liftState().targetPosition());
    EXPECT_EQ(Position::fullyOpen(), cover.liftState().currentPosition());

    EXPECT_EQ(3u, events.size());
    EXPECT_STREQ(CoverMarkedAsUnreachable::kEventName, events.peek()->eventName());

    auto& event = static_cast<const CoverMarkedAsUnreachable&>(*events.peek());

    EXPECT_EQ(cover.endpointId(), event.endpointId);
    EXPECT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);

    (void)events.pop();
    EXPECT_STREQ(CoverOperationalStatusChanged::kEventName, events.peek()->eventName());

    (void)events.pop();
    EXPECT_STREQ(CoverLiftTargetPositionChanged::kEventName, events.peek()->eventName());

    (void)events.pop();

    cover.markAsUnreachable();
    EXPECT_EQ(0u, events.size());
}

TEST(CoverTest, MarksAsReachableAfterLiftPositionChanged)
{
    auto cover = coverStub();

    cover.markAsUnreachable();
    EXPECT_FALSE(cover.isReachable());

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.changeLiftPosition(Position::fullyClosed());

    EXPECT_TRUE(r.has_value());
    EXPECT_TRUE(cover.isReachable());
    EXPECT_EQ(CoverMotion::NotMoving, cover.operationalStatus().lift());
    EXPECT_EQ(CoverMotion::NotMoving, cover.operationalStatus().tilt());
    EXPECT_EQ(PositionStatus::Idle, cover.liftState().status());
    EXPECT_EQ(CoverMotion::NotMoving, cover.liftState().motion());
    EXPECT_EQ(Position::fullyClosed(), cover.liftState().targetPosition());
    EXPECT_EQ(Position::fullyClosed(), cover.liftState().currentPosition());

    EXPECT_EQ(3u, events.size());
    EXPECT_STREQ(CoverMarkedAsReachable::kEventName, events.peek()->eventName());

    auto& event = static_cast<const CoverMarkedAsReachable&>(*events.peek());

    EXPECT_EQ(cover.endpointId(), event.endpointId);
    EXPECT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);

    (void)events.pop();
    EXPECT_STREQ(CoverLiftTargetPositionChanged::kEventName, events.peek()->eventName());

    (void)events.pop();
    EXPECT_STREQ(CoverLiftCurrentPositionChanged::kEventName, events.peek()->eventName());

    (void)events.pop();
}

TEST(CoverTest, FailMotionResetsLiftState)
{
    auto cover = coverStub();
    EXPECT_TRUE(cover.startLiftTo(Position::fullyClosed()));

    auto& events = DomainEventQueue::instance();
    events.clear();

    cover.failMotion();
    EXPECT_EQ(CoverMotion::NotMoving, cover.operationalStatus().lift());
    EXPECT_EQ(CoverMotion::NotMoving, cover.operationalStatus().tilt());
    EXPECT_EQ(PositionStatus::Idle, cover.liftState().status());
    EXPECT_EQ(CoverMotion::NotMoving, cover.liftState().motion());
    EXPECT_EQ(Position::fullyOpen(), cover.liftState().targetPosition());
    EXPECT_EQ(Position::fullyOpen(), cover.liftState().currentPosition());

    EXPECT_EQ(2u, events.size());
    EXPECT_STREQ(CoverOperationalStatusChanged::kEventName, events.peek()->eventName());

    (void)events.pop();
    EXPECT_STREQ(CoverLiftTargetPositionChanged::kEventName, events.peek()->eventName());

    (void)events.pop();
}

TEST(CoverTest, Removes)
{
    auto cover = coverStub();
    auto& events = DomainEventQueue::instance();
    events.clear();

    cover.remove();

    EXPECT_EQ(1u, events.size());
    EXPECT_STREQ(CoverRemoved::kEventName, events.peek()->eventName());

    auto& event = static_cast<const CoverRemoved&>(*events.peek());

    EXPECT_EQ(cover.endpointId(), event.endpointId);
    EXPECT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);

    (void)events.pop();
}
