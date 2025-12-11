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

using namespace mobmatter::common::domain;
using namespace mobmatter::application::model;
using namespace mobmatter::application::model::window_covering;

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

    ASSERT_EQ(1, cover.endpointId());
    ASSERT_EQ(2, cover.mobilusDeviceId());
    ASSERT_TRUE(cover.isReachable());
    ASSERT_EQ("foo", cover.name());
    ASSERT_EQ(PositionStatus::Idle, cover.liftState().status());
    ASSERT_EQ(CoverMotion::NotMoving, cover.liftState().motion());
    ASSERT_EQ(Position::fullyOpen(), *cover.liftState().targetPosition());
    ASSERT_EQ(Position::fullyOpen(), *cover.liftState().currentPosition());
    ASSERT_EQ(CoverSpecification::Senso(), cover.specification());

    ASSERT_EQ(1u, events.size());
    ASSERT_STREQ(CoverAdded::kEventName, events.peek()->eventName());

    auto& event = static_cast<const CoverAdded&>(*events.peek());

    ASSERT_EQ(cover.endpointId(), event.endpointId);
    ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
    ASSERT_EQ(cover.specification(), event.specification);

    (void)events.pop();
}

TEST(CoverTest, Restores)
{
    auto cover = Cover::restoreFrom(1, 2, UniqueId::of("7bc1ac82347f4f64970db8228ed24290"), false, "foo", PositionState::at(Position::fullyClosed()), CoverSpecification::Cmr());
    auto& events = DomainEventQueue::instance();

    ASSERT_EQ(1, cover.endpointId());
    ASSERT_EQ(2, cover.mobilusDeviceId());
    ASSERT_EQ("7bc1ac82347f4f64970db8228ed24290", cover.uniqueId().value());
    ASSERT_FALSE(cover.isReachable());
    ASSERT_EQ("foo", cover.name());
    ASSERT_EQ(PositionStatus::Idle, cover.liftState().status());
    ASSERT_EQ(CoverMotion::NotMoving, cover.liftState().motion());
    ASSERT_EQ(Position::fullyClosed(), *cover.liftState().targetPosition());
    ASSERT_EQ(Position::fullyClosed(), *cover.liftState().currentPosition());
    ASSERT_EQ(CoverSpecification::Cmr(), cover.specification());
    ASSERT_EQ(0u, events.size());
}

TEST(CoverTest, EqualsAndDoesNotEqual)
{
    auto cover = coverStub();
    auto other = nonLiftCoverStub();

    ASSERT_EQ(cover, coverStub(Position::fullyClosed()));
    ASSERT_FALSE(cover == other);
}

TEST(CoverTest, RequestsLiftToPosition)
{
    auto cover = coverStub();

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.requestLiftTo(Position::fullyClosed());

    ASSERT_EQ(Cover::Result::Ok, r);
    ASSERT_EQ(PositionStatus::Requested, cover.liftState().status());
    ASSERT_EQ(CoverMotion::NotMoving, cover.liftState().motion());
    ASSERT_EQ(Position::fullyClosed(), cover.liftState().targetPosition());
    ASSERT_EQ(Position::fullyOpen(), cover.liftState().currentPosition());

    ASSERT_EQ(2u, events.size());
    ASSERT_STREQ(CoverLiftTargetPositionChanged::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverLiftTargetPositionChanged&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        ASSERT_EQ(cover.liftState().targetPosition(), event.position);
    }

    (void)events.pop();
    ASSERT_STREQ(CoverLiftRequested::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverLiftRequested&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        ASSERT_EQ(cover.liftState().targetPosition(), event.position);
    }

    (void)events.pop();

    auto otherR = cover.requestLiftTo(Position::fullyClosed());

    ASSERT_EQ(Cover::Result::NoChange, otherR);
    ASSERT_EQ(0u, events.size());
}

TEST(CoverTest, RequestsLiftToPositionFailsForNonLiftCover)
{
    auto cover = nonLiftCoverStub();

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.requestLiftTo(Position::fullyClosed());

    ASSERT_EQ(Cover::Result::LiftNotSupported, r);
    ASSERT_EQ(PositionStatus::Unavailable, cover.liftState().status());
    ASSERT_EQ(0u, events.size());
}

TEST(CoverTest, RequestsOpen)
{
    auto cover = coverStub(Position::fullyClosed());

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.requestOpen();

    ASSERT_EQ(Cover::Result::Ok, r);
    ASSERT_EQ(PositionStatus::Requested, cover.liftState().status());
    ASSERT_EQ(CoverMotion::NotMoving, cover.liftState().motion());
    ASSERT_EQ(Position::fullyOpen(), cover.liftState().targetPosition());
    ASSERT_EQ(Position::fullyClosed(), cover.liftState().currentPosition());

    ASSERT_EQ(2u, events.size());
    ASSERT_STREQ(CoverLiftTargetPositionChanged::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverLiftTargetPositionChanged&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        ASSERT_EQ(cover.liftState().targetPosition(), event.position);
    }

    (void)events.pop();
    ASSERT_STREQ(CoverLiftRequested::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverLiftRequested&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        ASSERT_EQ(cover.liftState().targetPosition(), event.position);
    }

    (void)events.pop();

    ASSERT_EQ(Cover::Result::NoChange, cover.requestOpen());
    ASSERT_EQ(0u, events.size());
}

TEST(CoverTest, RequestsClose)
{
    auto cover = coverStub();

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.requestClose();

    ASSERT_EQ(Cover::Result::Ok, r);
    ASSERT_EQ(PositionStatus::Requested, cover.liftState().status());
    ASSERT_EQ(CoverMotion::NotMoving, cover.liftState().motion());
    ASSERT_EQ(Position::fullyClosed(), cover.liftState().targetPosition());
    ASSERT_EQ(Position::fullyOpen(), cover.liftState().currentPosition());

    ASSERT_EQ(2u, events.size());
    ASSERT_STREQ(CoverLiftTargetPositionChanged::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverLiftTargetPositionChanged&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        ASSERT_EQ(cover.liftState().targetPosition(), event.position);
    }

    (void)events.pop();
    ASSERT_STREQ(CoverLiftRequested::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverLiftRequested&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        ASSERT_EQ(cover.liftState().targetPosition(), event.position);
    }

    (void)events.pop();

    ASSERT_EQ(Cover::Result::NoChange, cover.requestClose());
    ASSERT_EQ(0u, events.size());
}

TEST(CoverTest, RequestsStopMotion)
{
    auto cover = coverStub();
    ASSERT_EQ(Cover::Result::Ok, cover.reportLiftTo(Position::fullyClosed()));

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.requestStopMotion();

    ASSERT_EQ(Cover::Result::Ok, r);
    ASSERT_EQ(PositionStatus::Stopping, cover.liftState().status());
    ASSERT_EQ(CoverMotion::Closing, cover.liftState().motion());
    ASSERT_EQ(Position::fullyClosed(), cover.liftState().targetPosition());
    ASSERT_EQ(Position::fullyOpen(), cover.liftState().currentPosition());

    ASSERT_EQ(1u, events.size());
    ASSERT_STREQ(CoverStopMotionRequested::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverStopMotionRequested&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
    }

    (void)events.pop();

    ASSERT_EQ(Cover::Result::NoChange, cover.requestStopMotion());
    ASSERT_EQ(0u, events.size());
}

TEST(CoverTest, RequestsStopMotionAfterLiftRequest)
{
    auto cover = coverStub();
    ASSERT_EQ(Cover::Result::Ok, cover.requestLiftTo(Position::fullyClosed()));

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.requestStopMotion();

    ASSERT_EQ(Cover::Result::Ok, r);
    ASSERT_EQ(PositionStatus::Stopping, cover.liftState().status());
    ASSERT_EQ(CoverMotion::NotMoving, cover.liftState().motion());
    ASSERT_EQ(Position::fullyClosed(), cover.liftState().targetPosition());
    ASSERT_EQ(Position::fullyOpen(), cover.liftState().currentPosition());

    ASSERT_EQ(1u, events.size());
    ASSERT_STREQ(CoverStopMotionRequested::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverStopMotionRequested&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
    }

    (void)events.pop();

    ASSERT_EQ(Cover::Result::NoChange, cover.requestStopMotion());
    ASSERT_EQ(0u, events.size());
}

TEST(CoverTest, RequestsStopMotionDoesNothingIfNotInMotion)
{
    auto cover = coverStub();
    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.requestStopMotion();

    ASSERT_EQ(Cover::Result::NoChange, r);
    ASSERT_EQ(PositionStatus::Idle, cover.liftState().status());
    ASSERT_EQ(CoverMotion::NotMoving, cover.liftState().motion());
    ASSERT_EQ(0u, events.size());
}

TEST(CoverTest, ReportsStopMotion)
{
    auto cover = coverStub();
    ASSERT_EQ(Cover::Result::Ok, cover.reportLiftTo(Position::fullyClosed()));

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.reportStopMotion();

    ASSERT_EQ(Cover::Result::Ok, r);
    ASSERT_EQ(PositionStatus::Stopping, cover.liftState().status());
    ASSERT_EQ(CoverMotion::Closing, cover.liftState().motion());
    ASSERT_EQ(Position::fullyClosed(), cover.liftState().targetPosition());
    ASSERT_EQ(Position::fullyOpen(), cover.liftState().currentPosition());
    ASSERT_EQ(0u, events.size());
}

TEST(CoverTest, ReportsLiftToClosePosition)
{
    auto cover = coverStub();

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.reportLiftTo(Position::fullyClosed());

    ASSERT_EQ(Cover::Result::Ok, r);
    ASSERT_EQ(CoverMotion::Closing, cover.operationalStatus().lift());
    ASSERT_EQ(CoverMotion::NotMoving, cover.operationalStatus().tilt());
    ASSERT_EQ(PositionStatus::Moving, cover.liftState().status());
    ASSERT_EQ(CoverMotion::Closing, cover.liftState().motion());
    ASSERT_EQ(Position::fullyClosed(), cover.liftState().targetPosition());
    ASSERT_EQ(Position::fullyOpen(), cover.liftState().currentPosition());

    ASSERT_EQ(2u, events.size());
    ASSERT_STREQ(CoverOperationalStatusChanged::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverOperationalStatusChanged&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        ASSERT_EQ(cover.operationalStatus(), event.operationalStatus);
    }

    (void)events.pop();
    ASSERT_STREQ(CoverLiftTargetPositionChanged::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverLiftTargetPositionChanged&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        ASSERT_EQ(cover.liftState().targetPosition(), event.position);
    }

    (void)events.pop();

    auto otherR = cover.reportLiftTo(*cover.liftState().targetPosition());

    ASSERT_EQ(Cover::Result::NoChange, otherR);
    ASSERT_EQ(0u, events.size());
}

TEST(CoverTest, ReportsLiftToOpenPosition)
{
    auto cover = coverStub(Position::fullyClosed());

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.reportLiftTo(Position::fullyOpen());

    ASSERT_EQ(Cover::Result::Ok, r);
    ASSERT_EQ(CoverMotion::Opening, cover.operationalStatus().lift());
    ASSERT_EQ(CoverMotion::NotMoving, cover.operationalStatus().tilt());
    ASSERT_EQ(PositionStatus::Moving, cover.liftState().status());
    ASSERT_EQ(CoverMotion::Opening, cover.liftState().motion());
    ASSERT_EQ(Position::fullyOpen(), cover.liftState().targetPosition());
    ASSERT_EQ(Position::fullyClosed(), cover.liftState().currentPosition());

    ASSERT_EQ(2u, events.size());
    ASSERT_STREQ(CoverOperationalStatusChanged::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverOperationalStatusChanged&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        ASSERT_EQ(cover.operationalStatus(), event.operationalStatus);
    }

    (void)events.pop();
    ASSERT_STREQ(CoverLiftTargetPositionChanged::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverLiftTargetPositionChanged&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        ASSERT_EQ(cover.liftState().targetPosition(), event.position);
    }

    (void)events.pop();

    auto otherR = cover.reportLiftTo(*cover.liftState().targetPosition());

    ASSERT_EQ(Cover::Result::NoChange, otherR);
    ASSERT_EQ(0u, events.size());
}

TEST(CoverTest, ReportsLiftToPositionAfterRequest)
{
    auto cover = coverStub();
    ASSERT_EQ(Cover::Result::Ok, cover.requestClose());

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.reportLiftTo(Position::fullyClosed());

    ASSERT_EQ(Cover::Result::Ok, r);
    ASSERT_EQ(CoverMotion::Closing, cover.operationalStatus().lift());
    ASSERT_EQ(CoverMotion::NotMoving, cover.operationalStatus().tilt());
    ASSERT_EQ(PositionStatus::Moving, cover.liftState().status());
    ASSERT_EQ(CoverMotion::Closing, cover.liftState().motion());
    ASSERT_EQ(Position::fullyClosed(), cover.liftState().targetPosition());
    ASSERT_EQ(Position::fullyOpen(), cover.liftState().currentPosition());

    ASSERT_EQ(1u, events.size());
    ASSERT_STREQ(CoverOperationalStatusChanged::kEventName, events.peek()->eventName());

    auto& event = static_cast<const CoverOperationalStatusChanged&>(*events.peek());

    ASSERT_EQ(cover.endpointId(), event.endpointId);
    ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
    ASSERT_EQ(cover.operationalStatus(), event.operationalStatus);

    (void)events.pop();
}

TEST(CoverTest, ReportLiftToPositionFailsForNonLiftCover)
{
    auto cover = nonLiftCoverStub();

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.reportLiftTo(Position::fullyClosed());

    ASSERT_EQ(Cover::Result::LiftNotSupported, r);
    ASSERT_EQ(PositionStatus::Unavailable, cover.liftState().status());
    ASSERT_EQ(0u, events.size());
}

TEST(CoverTest, SyncesLiftPosition)
{
    auto cover = coverStub();

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.reportLiftPosition(Position::fullyClosed());

    ASSERT_EQ(Cover::Result::Ok, r);
    ASSERT_EQ(CoverMotion::NotMoving, cover.operationalStatus().lift());
    ASSERT_EQ(CoverMotion::NotMoving, cover.operationalStatus().tilt());
    ASSERT_EQ(PositionStatus::Idle, cover.liftState().status());
    ASSERT_EQ(CoverMotion::NotMoving, cover.liftState().motion());
    ASSERT_EQ(Position::fullyClosed(), cover.liftState().targetPosition());
    ASSERT_EQ(Position::fullyClosed(), cover.liftState().currentPosition());

    ASSERT_EQ(2u, events.size());
    ASSERT_STREQ(CoverLiftTargetPositionChanged::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverLiftTargetPositionChanged&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        ASSERT_EQ(*cover.liftState().targetPosition(), event.position);
    }

    (void)events.pop();
    ASSERT_STREQ(CoverLiftCurrentPositionChanged::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverLiftCurrentPositionChanged&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        ASSERT_EQ(*cover.liftState().currentPosition(), event.position);
    }

    (void)events.pop();

    auto otherR = cover.reportLiftPosition(*cover.liftState().currentPosition());
    ASSERT_EQ(Cover::Result::NoChange, otherR);
    ASSERT_EQ(0u, events.size());
}

TEST(CoverTest, ChangesLiftPositionAfterMoveStarted)
{
    auto cover = coverStub();
    ASSERT_EQ(Cover::Result::Ok, cover.reportLiftTo(Position::fullyClosed()));

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.reportLiftPosition(Position::fullyClosed());

    ASSERT_EQ(Cover::Result::Ok, r);
    ASSERT_EQ(CoverMotion::NotMoving, cover.operationalStatus().lift());
    ASSERT_EQ(CoverMotion::NotMoving, cover.operationalStatus().tilt());
    ASSERT_EQ(PositionStatus::Idle, cover.liftState().status());
    ASSERT_EQ(CoverMotion::NotMoving, cover.liftState().motion());
    ASSERT_EQ(Position::fullyClosed(), cover.liftState().targetPosition());
    ASSERT_EQ(Position::fullyClosed(), cover.liftState().currentPosition());

    ASSERT_EQ(2u, events.size());
    ASSERT_STREQ(CoverOperationalStatusChanged::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverOperationalStatusChanged&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        ASSERT_EQ(cover.operationalStatus(), event.operationalStatus);
    }

    (void)events.pop();
    ASSERT_STREQ(CoverLiftCurrentPositionChanged::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverLiftCurrentPositionChanged&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        ASSERT_EQ(*cover.liftState().currentPosition(), event.position);
    }

    (void)events.pop();
}

TEST(CoverTest, ChangesLiftPositionAfterMoveRequest)
{
    auto cover = coverStub();
    ASSERT_EQ(Cover::Result::Ok, cover.requestLiftTo(Position::fullyClosed()));

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.reportLiftPosition(Position::fullyClosed());

    ASSERT_EQ(Cover::Result::Ok, r);
    ASSERT_EQ(CoverMotion::NotMoving, cover.operationalStatus().lift());
    ASSERT_EQ(CoverMotion::NotMoving, cover.operationalStatus().tilt());
    ASSERT_EQ(PositionStatus::Idle, cover.liftState().status());
    ASSERT_EQ(CoverMotion::NotMoving, cover.liftState().motion());
    ASSERT_EQ(Position::fullyClosed(), cover.liftState().targetPosition());
    ASSERT_EQ(Position::fullyClosed(), cover.liftState().currentPosition());

    ASSERT_EQ(1u, events.size());
    ASSERT_STREQ(CoverLiftCurrentPositionChanged::kEventName, events.peek()->eventName());

    auto& event = static_cast<const CoverLiftCurrentPositionChanged&>(*events.peek());

    ASSERT_EQ(cover.endpointId(), event.endpointId);
    ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
    ASSERT_EQ(*cover.liftState().currentPosition(), event.position);

    (void)events.pop();
}

TEST(CoverTest, ChangesLiftPositionToPrevious)
{
    auto cover = coverStub();
    ASSERT_EQ(Cover::Result::Ok, cover.requestLiftTo(Position::fullyClosed()));

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.reportLiftPosition(*cover.liftState().currentPosition());

    ASSERT_EQ(Cover::Result::Ok, r);
    ASSERT_EQ(CoverMotion::NotMoving, cover.operationalStatus().lift());
    ASSERT_EQ(CoverMotion::NotMoving, cover.operationalStatus().tilt());
    ASSERT_EQ(PositionStatus::Idle, cover.liftState().status());
    ASSERT_EQ(CoverMotion::NotMoving, cover.liftState().motion());
    ASSERT_EQ(cover.liftState().currentPosition(), cover.liftState().targetPosition());

    ASSERT_EQ(1u, events.size());
    ASSERT_STREQ(CoverLiftTargetPositionChanged::kEventName, events.peek()->eventName());

    auto& event = static_cast<const CoverLiftTargetPositionChanged&>(*events.peek());

    ASSERT_EQ(cover.endpointId(), event.endpointId);
    ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
    ASSERT_EQ(*cover.liftState().targetPosition(), event.position);

    (void)events.pop();
}

TEST(CoverTest, ReportsCoverIsUnreachable)
{
    auto cover = coverStub();

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.reportUnreachable();

    ASSERT_EQ(Cover::Result::Ok, r);
    ASSERT_FALSE(cover.isReachable());

    ASSERT_EQ(1u, events.size());
    ASSERT_STREQ(CoverMarkedAsUnreachable::kEventName, events.peek()->eventName());

    auto& event = static_cast<const CoverMarkedAsUnreachable&>(*events.peek());

    ASSERT_EQ(cover.endpointId(), event.endpointId);
    ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);

    (void)events.pop();

    ASSERT_EQ(Cover::Result::NoChange, cover.reportUnreachable());
    ASSERT_EQ(0u, events.size());
}

TEST(CoverTest, ReportsCoverIsUnreachableAndStopsMoving)
{
    auto cover = coverStub();
    ASSERT_EQ(Cover::Result::Ok, cover.reportLiftTo(Position::fullyClosed()));

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.reportUnreachable();

    ASSERT_EQ(Cover::Result::Ok, r);
    ASSERT_FALSE(cover.isReachable());
    ASSERT_EQ(CoverMotion::NotMoving, cover.operationalStatus().lift());
    ASSERT_EQ(CoverMotion::NotMoving, cover.operationalStatus().tilt());
    ASSERT_EQ(PositionStatus::Idle, cover.liftState().status());
    ASSERT_EQ(CoverMotion::NotMoving, cover.liftState().motion());
    ASSERT_EQ(Position::fullyOpen(), cover.liftState().targetPosition());
    ASSERT_EQ(Position::fullyOpen(), cover.liftState().currentPosition());

    ASSERT_EQ(3u, events.size());

    ASSERT_STREQ(CoverMarkedAsUnreachable::kEventName, events.peek()->eventName());
    (void)events.pop();

    ASSERT_STREQ(CoverOperationalStatusChanged::kEventName, events.peek()->eventName());
    (void)events.pop();

    ASSERT_STREQ(CoverLiftTargetPositionChanged::kEventName, events.peek()->eventName());
    (void)events.pop();
}

TEST(CoverTest, ReportsCoverIsReachable)
{
    auto cover = coverStub();

    ASSERT_EQ(Cover::Result::Ok, cover.reportUnreachable());
    ASSERT_FALSE(cover.isReachable());

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.reportLiftPosition(*cover.liftState().currentPosition());

    ASSERT_EQ(Cover::Result::Ok, r);
    ASSERT_TRUE(cover.isReachable());

    ASSERT_EQ(1u, events.size());
    ASSERT_STREQ(CoverMarkedAsReachable::kEventName, events.peek()->eventName());

    auto& event = static_cast<const CoverMarkedAsReachable&>(*events.peek());

    ASSERT_EQ(cover.endpointId(), event.endpointId);
    ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);

    (void)events.pop();
}

TEST(CoverTest, ReportsCoverIsReachableAndSyncesLiftPosition)
{
    auto cover = coverStub();

    ASSERT_EQ(Cover::Result::Ok, cover.reportUnreachable());
    ASSERT_FALSE(cover.isReachable());

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.reportLiftPosition(Position::fullyClosed());

    ASSERT_EQ(Cover::Result::Ok, r);
    ASSERT_TRUE(cover.isReachable());
    ASSERT_EQ(CoverMotion::NotMoving, cover.operationalStatus().lift());
    ASSERT_EQ(CoverMotion::NotMoving, cover.operationalStatus().tilt());
    ASSERT_EQ(PositionStatus::Idle, cover.liftState().status());
    ASSERT_EQ(CoverMotion::NotMoving, cover.liftState().motion());
    ASSERT_EQ(Position::fullyClosed(), cover.liftState().targetPosition());
    ASSERT_EQ(Position::fullyClosed(), cover.liftState().currentPosition());

    ASSERT_EQ(3u, events.size());

    ASSERT_STREQ(CoverMarkedAsReachable::kEventName, events.peek()->eventName());
    (void)events.pop();

    ASSERT_STREQ(CoverLiftTargetPositionChanged::kEventName, events.peek()->eventName());
    (void)events.pop();

    ASSERT_STREQ(CoverLiftCurrentPositionChanged::kEventName, events.peek()->eventName());
    (void)events.pop();
}

TEST(CoverTest, ReportsMotionFailure)
{
    auto cover = coverStub();
    ASSERT_EQ(Cover::Result::Ok, cover.reportLiftTo(Position::fullyClosed()));

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.reportMotionFailure();

    ASSERT_EQ(Cover::Result::Ok, r);
    ASSERT_EQ(CoverMotion::NotMoving, cover.operationalStatus().lift());
    ASSERT_EQ(CoverMotion::NotMoving, cover.operationalStatus().tilt());
    ASSERT_EQ(PositionStatus::Idle, cover.liftState().status());
    ASSERT_EQ(CoverMotion::NotMoving, cover.liftState().motion());
    ASSERT_EQ(Position::fullyOpen(), cover.liftState().targetPosition());
    ASSERT_EQ(Position::fullyOpen(), cover.liftState().currentPosition());

    ASSERT_EQ(2u, events.size());

    ASSERT_STREQ(CoverOperationalStatusChanged::kEventName, events.peek()->eventName());
    (void)events.pop();

    ASSERT_STREQ(CoverLiftTargetPositionChanged::kEventName, events.peek()->eventName());
    (void)events.pop();
}

TEST(CoverTest, ReportMotionFailureDoesNothingIfNotMoving)
{
    auto cover = coverStub();

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.reportMotionFailure();

    ASSERT_EQ(Cover::Result::NoChange, r);
    ASSERT_EQ(0u, events.size());
}

TEST(CoverTest, ReportsRemoved)
{
    auto cover = coverStub();
    auto& events = DomainEventQueue::instance();
    events.clear();

    cover.reportRemoved();

    ASSERT_EQ(1u, events.size());
    ASSERT_STREQ(CoverRemoved::kEventName, events.peek()->eventName());

    auto& event = static_cast<const CoverRemoved&>(*events.peek());

    ASSERT_EQ(cover.endpointId(), event.endpointId);
    ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);

    (void)events.pop();
}
