#include "application/model/window_covering/Cover.h"
#include "application/model/window_covering/CoverAdded.h"
#include "application/model/window_covering/CoverCloseRequested.h"
#include "application/model/window_covering/CoverLiftCurrentPositionChanged.h"
#include "application/model/window_covering/CoverLiftMotionChanged.h"
#include "application/model/window_covering/CoverLiftRequested.h"
#include "application/model/window_covering/CoverLiftTargetPositionChanged.h"
#include "application/model/window_covering/CoverMarkedAsReachable.h"
#include "application/model/window_covering/CoverMarkedAsUnreachable.h"
#include "application/model/window_covering/CoverOpenRequested.h"
#include "application/model/window_covering/CoverRemoved.h"
#include "application/model/window_covering/CoverStopMotionRequested.h"
#include "application/model/window_covering/CoverTiltCurrentPositionChanged.h"
#include "application/model/window_covering/CoverTiltMotionChanged.h"
#include "application/model/window_covering/CoverTiltRequested.h"
#include "application/model/window_covering/CoverTiltTargetPositionChanged.h"
#include "common/domain/DomainEventQueue.h"

#include <gtest/gtest.h>

using namespace mobmatter::common::domain;
using namespace mobmatter::application::model;
using namespace mobmatter::application::model::window_covering;

namespace {

auto liftAndTiltCover(Position liftPosition = Position::fullyOpen(), Position tiltPosition = Position::fullyOpen())
{
    return Cover::add(1, 11, CoverSpecification::SensoZ(), "lift_tilt", PositionState::at(liftPosition), PositionState::at(tiltPosition));
}

auto liftCover(Position position = Position::fullyOpen())
{
    return Cover::add(2, 12, CoverSpecification::Senso(), "lift", PositionState::at(position), PositionState::unavailable());
}

auto tiltCover(Position position = Position::fullyOpen())
{
    return Cover::add(3, 13, CoverSpecification::SensoZ(), "tilt", PositionState::unavailable(), PositionState::at(position));
}

}

TEST(CoverTest, AddsNew)
{
    auto cover = liftAndTiltCover();
    auto& events = DomainEventQueue::instance();

    ASSERT_EQ(1, cover.endpointId());
    ASSERT_EQ(11, cover.mobilusDeviceId());
    ASSERT_EQ(CoverSpecification::SensoZ(), cover.specification());
    ASSERT_TRUE(cover.isReachable());
    ASSERT_EQ("lift_tilt", cover.name());
    ASSERT_EQ(PositionStatus::Idle, cover.liftState().status());
    ASSERT_EQ(CoverMotion::NotMoving, cover.liftState().motion());
    ASSERT_EQ(Position::fullyOpen(), *cover.liftState().targetPosition());
    ASSERT_EQ(Position::fullyOpen(), *cover.liftState().currentPosition());
    ASSERT_EQ(PositionStatus::Idle, cover.tiltState().status());
    ASSERT_EQ(CoverMotion::NotMoving, cover.tiltState().motion());
    ASSERT_EQ(Position::fullyOpen(), *cover.tiltState().targetPosition());
    ASSERT_EQ(Position::fullyOpen(), *cover.tiltState().currentPosition());

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
    auto cover = Cover::restoreFrom(1, 11, UniqueId::of("7bc1ac82347f4f64970db8228ed24290"), CoverSpecification::SensoZ(), false, "foo", PositionState::at(Position::fullyClosed()), PositionState::at(Position::fullyOpen()));
    auto& events = DomainEventQueue::instance();

    ASSERT_EQ(1, cover.endpointId());
    ASSERT_EQ(11, cover.mobilusDeviceId());
    ASSERT_EQ("7bc1ac82347f4f64970db8228ed24290", cover.uniqueId().value());
    ASSERT_EQ(CoverSpecification::SensoZ(), cover.specification());
    ASSERT_FALSE(cover.isReachable());
    ASSERT_EQ("foo", cover.name());
    ASSERT_EQ(PositionStatus::Idle, cover.liftState().status());
    ASSERT_EQ(CoverMotion::NotMoving, cover.liftState().motion());
    ASSERT_EQ(Position::fullyClosed(), *cover.liftState().targetPosition());
    ASSERT_EQ(Position::fullyClosed(), *cover.liftState().currentPosition());
    ASSERT_EQ(PositionStatus::Idle, cover.tiltState().status());
    ASSERT_EQ(CoverMotion::NotMoving, cover.tiltState().motion());
    ASSERT_EQ(Position::fullyOpen(), *cover.tiltState().targetPosition());
    ASSERT_EQ(Position::fullyOpen(), *cover.tiltState().currentPosition());
    ASSERT_TRUE(events.empty());
}

TEST(CoverTest, EqualsAndDoesNotEqual)
{
    auto cover = liftCover();
    auto other = tiltCover();

    ASSERT_EQ(cover, liftCover(Position::fullyClosed()));
    ASSERT_FALSE(cover == other);
}

TEST(CoverTest, RequestsOpenForLiftAndTiltCover)
{
    auto cover = liftAndTiltCover(Position::fullyClosed(), Position::fullyClosed());

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.requestOpen();

    ASSERT_EQ(Cover::Result::Ok, r);
    ASSERT_EQ(PositionStatus::Moving, cover.liftState().status());
    ASSERT_EQ(CoverMotion::Opening, cover.liftState().motion());
    ASSERT_EQ(Position::fullyOpen(), cover.liftState().targetPosition());
    ASSERT_EQ(Position::fullyClosed(), cover.liftState().currentPosition());
    ASSERT_EQ(PositionStatus::Moving, cover.tiltState().status());
    ASSERT_EQ(CoverMotion::Opening, cover.tiltState().motion());
    ASSERT_EQ(Position::fullyOpen(), cover.tiltState().targetPosition());
    ASSERT_EQ(Position::fullyClosed(), cover.tiltState().currentPosition());

    ASSERT_EQ(5u, events.size());
    ASSERT_STREQ(CoverLiftMotionChanged::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverLiftMotionChanged&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        ASSERT_EQ(cover.liftState().motion(), event.motion);
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
    ASSERT_STREQ(CoverTiltMotionChanged::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverTiltMotionChanged&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        ASSERT_EQ(cover.tiltState().motion(), event.motion);
    }

    (void)events.pop();
    ASSERT_STREQ(CoverTiltTargetPositionChanged::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverTiltTargetPositionChanged&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        ASSERT_EQ(cover.tiltState().targetPosition(), event.position);
    }

    (void)events.pop();
    ASSERT_STREQ(CoverOpenRequested::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverOpenRequested&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
    }

    (void)events.pop();

    ASSERT_EQ(Cover::Result::NoChange, cover.requestOpen());
    ASSERT_TRUE(events.empty());
}

TEST(CoverTest, RequestsOpenForLiftCover)
{
    auto cover = liftCover(Position::fullyClosed());

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.requestOpen();

    ASSERT_EQ(Cover::Result::Ok, r);
    ASSERT_EQ(PositionStatus::Moving, cover.liftState().status());
    ASSERT_EQ(CoverMotion::Opening, cover.liftState().motion());
    ASSERT_EQ(Position::fullyOpen(), cover.liftState().targetPosition());
    ASSERT_EQ(Position::fullyClosed(), cover.liftState().currentPosition());
    ASSERT_EQ(PositionStatus::Unavailable, cover.tiltState().status());
    ASSERT_EQ(CoverMotion::NotMoving, cover.tiltState().motion());
    ASSERT_FALSE(cover.tiltState().targetPosition());
    ASSERT_FALSE(cover.tiltState().currentPosition());

    ASSERT_EQ(3u, events.size());
    ASSERT_STREQ(CoverLiftMotionChanged::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverLiftMotionChanged&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        ASSERT_EQ(cover.liftState().motion(), event.motion);
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
    ASSERT_STREQ(CoverOpenRequested::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverOpenRequested&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
    }

    (void)events.pop();

    ASSERT_EQ(Cover::Result::NoChange, cover.requestOpen());
    ASSERT_TRUE(events.empty());
}

TEST(CoverTest, RequestsOpenForTiltCover)
{
    auto cover = tiltCover(Position::fullyClosed());

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.requestOpen();

    ASSERT_EQ(Cover::Result::Ok, r);
    ASSERT_EQ(PositionStatus::Unavailable, cover.liftState().status());
    ASSERT_EQ(CoverMotion::NotMoving, cover.liftState().motion());
    ASSERT_FALSE(cover.liftState().targetPosition());
    ASSERT_FALSE(cover.liftState().currentPosition());
    ASSERT_EQ(PositionStatus::Moving, cover.tiltState().status());
    ASSERT_EQ(CoverMotion::Opening, cover.tiltState().motion());
    ASSERT_EQ(Position::fullyOpen(), cover.tiltState().targetPosition());
    ASSERT_EQ(Position::fullyClosed(), cover.tiltState().currentPosition());

    ASSERT_EQ(3u, events.size());
    ASSERT_STREQ(CoverTiltMotionChanged::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverTiltMotionChanged&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        ASSERT_EQ(cover.tiltState().motion(), event.motion);
    }

    (void)events.pop();
    ASSERT_STREQ(CoverTiltTargetPositionChanged::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverTiltTargetPositionChanged&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        ASSERT_EQ(cover.tiltState().targetPosition(), event.position);
    }

    (void)events.pop();
    ASSERT_STREQ(CoverOpenRequested::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverOpenRequested&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
    }

    (void)events.pop();

    ASSERT_EQ(Cover::Result::NoChange, cover.requestOpen());
    ASSERT_TRUE(events.empty());
}

TEST(CoverTest, RequestsCloseForLiftAndTiltCover)
{
    auto cover = liftAndTiltCover();

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.requestClose();

    ASSERT_EQ(Cover::Result::Ok, r);
    ASSERT_EQ(PositionStatus::Moving, cover.liftState().status());
    ASSERT_EQ(CoverMotion::Closing, cover.liftState().motion());
    ASSERT_EQ(Position::fullyClosed(), cover.liftState().targetPosition());
    ASSERT_EQ(Position::fullyOpen(), cover.liftState().currentPosition());
    ASSERT_EQ(PositionStatus::Moving, cover.tiltState().status());
    ASSERT_EQ(CoverMotion::Closing, cover.tiltState().motion());
    ASSERT_EQ(Position::fullyClosed(), cover.tiltState().targetPosition());
    ASSERT_EQ(Position::fullyOpen(), cover.tiltState().currentPosition());

    ASSERT_EQ(5u, events.size());
    ASSERT_STREQ(CoverLiftMotionChanged::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverLiftMotionChanged&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        ASSERT_EQ(cover.liftState().motion(), event.motion);
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
    ASSERT_STREQ(CoverTiltMotionChanged::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverTiltMotionChanged&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        ASSERT_EQ(cover.tiltState().motion(), event.motion);
    }

    (void)events.pop();
    ASSERT_STREQ(CoverTiltTargetPositionChanged::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverTiltTargetPositionChanged&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        ASSERT_EQ(cover.tiltState().targetPosition(), event.position);
    }

    (void)events.pop();
    ASSERT_STREQ(CoverCloseRequested::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverCloseRequested&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
    }

    (void)events.pop();

    ASSERT_EQ(Cover::Result::NoChange, cover.requestClose());
    ASSERT_TRUE(events.empty());
}

TEST(CoverTest, RequestsCloseForLiftCover)
{
    auto cover = liftCover();

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.requestClose();

    ASSERT_EQ(Cover::Result::Ok, r);
    ASSERT_EQ(PositionStatus::Moving, cover.liftState().status());
    ASSERT_EQ(CoverMotion::Closing, cover.liftState().motion());
    ASSERT_EQ(Position::fullyClosed(), cover.liftState().targetPosition());
    ASSERT_EQ(Position::fullyOpen(), cover.liftState().currentPosition());
    ASSERT_EQ(PositionStatus::Unavailable, cover.tiltState().status());
    ASSERT_EQ(CoverMotion::NotMoving, cover.tiltState().motion());
    ASSERT_FALSE(cover.tiltState().targetPosition());
    ASSERT_FALSE(cover.tiltState().currentPosition());

    ASSERT_EQ(3u, events.size());
    ASSERT_STREQ(CoverLiftMotionChanged::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverLiftMotionChanged&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        ASSERT_EQ(cover.liftState().motion(), event.motion);
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
    ASSERT_STREQ(CoverCloseRequested::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverCloseRequested&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
    }

    (void)events.pop();

    ASSERT_EQ(Cover::Result::NoChange, cover.requestClose());
    ASSERT_TRUE(events.empty());
}

TEST(CoverTest, RequestsCloseForTiltCover)
{
    auto cover = tiltCover();

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.requestClose();

    ASSERT_EQ(Cover::Result::Ok, r);
    ASSERT_EQ(PositionStatus::Unavailable, cover.liftState().status());
    ASSERT_EQ(CoverMotion::NotMoving, cover.liftState().motion());
    ASSERT_FALSE(cover.liftState().targetPosition());
    ASSERT_FALSE(cover.liftState().currentPosition());
    ASSERT_EQ(PositionStatus::Moving, cover.tiltState().status());
    ASSERT_EQ(CoverMotion::Closing, cover.tiltState().motion());
    ASSERT_EQ(Position::fullyClosed(), cover.tiltState().targetPosition());
    ASSERT_EQ(Position::fullyOpen(), cover.tiltState().currentPosition());

    ASSERT_EQ(3u, events.size());
    ASSERT_STREQ(CoverTiltMotionChanged::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverTiltMotionChanged&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        ASSERT_EQ(cover.tiltState().motion(), event.motion);
    }

    (void)events.pop();
    ASSERT_STREQ(CoverTiltTargetPositionChanged::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverTiltTargetPositionChanged&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        ASSERT_EQ(cover.tiltState().targetPosition(), event.position);
    }

    (void)events.pop();
    ASSERT_STREQ(CoverCloseRequested::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverCloseRequested&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
    }

    (void)events.pop();

    ASSERT_EQ(Cover::Result::NoChange, cover.requestClose());
    ASSERT_TRUE(events.empty());
}

TEST(CoverTest, RequestsLiftToOpeningPosition)
{
    auto cover = liftCover(Position::fullyClosed());

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.requestLiftTo(Position::fullyOpen());

    ASSERT_EQ(Cover::Result::Ok, r);
    ASSERT_EQ(PositionStatus::Moving, cover.liftState().status());
    ASSERT_EQ(CoverMotion::Opening, cover.liftState().motion());
    ASSERT_EQ(Position::fullyOpen(), cover.liftState().targetPosition());
    ASSERT_EQ(Position::fullyClosed(), cover.liftState().currentPosition());

    ASSERT_EQ(3u, events.size());
    ASSERT_STREQ(CoverLiftMotionChanged::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverLiftMotionChanged&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        ASSERT_EQ(cover.liftState().motion(), event.motion);
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
    ASSERT_STREQ(CoverLiftRequested::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverLiftRequested&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        ASSERT_EQ(cover.liftState().targetPosition(), event.position);
    }

    (void)events.pop();

    ASSERT_EQ(Cover::Result::NoChange, cover.requestLiftTo(Position::fullyOpen()));
    ASSERT_TRUE(events.empty());
}

TEST(CoverTest, RequestsLiftToClosingPosition)
{
    auto cover = liftCover();

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.requestLiftTo(Position::fullyClosed());

    ASSERT_EQ(Cover::Result::Ok, r);
    ASSERT_EQ(PositionStatus::Moving, cover.liftState().status());
    ASSERT_EQ(CoverMotion::Closing, cover.liftState().motion());
    ASSERT_EQ(Position::fullyClosed(), cover.liftState().targetPosition());
    ASSERT_EQ(Position::fullyOpen(), cover.liftState().currentPosition());

    ASSERT_EQ(3u, events.size());
    ASSERT_STREQ(CoverLiftMotionChanged::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverLiftMotionChanged&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        ASSERT_EQ(cover.liftState().motion(), event.motion);
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
    ASSERT_STREQ(CoverLiftRequested::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverLiftRequested&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        ASSERT_EQ(cover.liftState().targetPosition(), event.position);
    }

    (void)events.pop();

    ASSERT_EQ(Cover::Result::NoChange, cover.requestLiftTo(Position::fullyClosed()));
    ASSERT_TRUE(events.empty());
}

TEST(CoverTest, RequestLiftToPositionFailsForTiltCover)
{
    auto cover = tiltCover();

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.requestLiftTo(Position::fullyClosed());

    ASSERT_EQ(Cover::Result::NotSupported, r);
    ASSERT_EQ(PositionStatus::Unavailable, cover.liftState().status());
    ASSERT_TRUE(events.empty());
}

TEST(CoverTest, RequestsTiltToOpeningPosition)
{
    auto cover = tiltCover(Position::fullyClosed());

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.requestTiltTo(Position::fullyOpen());

    ASSERT_EQ(Cover::Result::Ok, r);
    ASSERT_EQ(PositionStatus::Moving, cover.tiltState().status());
    ASSERT_EQ(CoverMotion::Opening, cover.tiltState().motion());
    ASSERT_EQ(Position::fullyOpen(), cover.tiltState().targetPosition());
    ASSERT_EQ(Position::fullyClosed(), cover.tiltState().currentPosition());

    ASSERT_EQ(3u, events.size());
    ASSERT_STREQ(CoverTiltMotionChanged::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverTiltMotionChanged&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        ASSERT_EQ(cover.tiltState().motion(), event.motion);
    }

    (void)events.pop();
    ASSERT_STREQ(CoverTiltTargetPositionChanged::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverTiltTargetPositionChanged&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        ASSERT_EQ(cover.tiltState().targetPosition(), event.position);
    }

    (void)events.pop();
    ASSERT_STREQ(CoverTiltRequested::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverTiltRequested&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        ASSERT_EQ(cover.tiltState().targetPosition(), event.position);
    }

    (void)events.pop();

    ASSERT_EQ(Cover::Result::NoChange, cover.requestTiltTo(Position::fullyOpen()));
    ASSERT_TRUE(events.empty());
}

TEST(CoverTest, RequestsTiltToClosingPosition)
{
    auto cover = tiltCover();

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.requestTiltTo(Position::fullyClosed());

    ASSERT_EQ(Cover::Result::Ok, r);
    ASSERT_EQ(PositionStatus::Moving, cover.tiltState().status());
    ASSERT_EQ(CoverMotion::Closing, cover.tiltState().motion());
    ASSERT_EQ(Position::fullyClosed(), cover.tiltState().targetPosition());
    ASSERT_EQ(Position::fullyOpen(), cover.tiltState().currentPosition());

    ASSERT_EQ(3u, events.size());
    ASSERT_STREQ(CoverTiltMotionChanged::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverTiltMotionChanged&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        ASSERT_EQ(cover.tiltState().motion(), event.motion);
    }

    (void)events.pop();
    ASSERT_STREQ(CoverTiltTargetPositionChanged::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverTiltTargetPositionChanged&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        ASSERT_EQ(cover.tiltState().targetPosition(), event.position);
    }

    (void)events.pop();
    ASSERT_STREQ(CoverTiltRequested::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverTiltRequested&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        ASSERT_EQ(cover.tiltState().targetPosition(), event.position);
    }

    (void)events.pop();

    ASSERT_EQ(Cover::Result::NoChange, cover.requestTiltTo(Position::fullyClosed()));
    ASSERT_TRUE(events.empty());
}

TEST(CoverTest, RequestTiltToPositionFailsForLiftCover)
{
    auto cover = liftCover();

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.requestTiltTo(Position::fullyClosed());

    ASSERT_EQ(Cover::Result::NotSupported, r);
    ASSERT_EQ(PositionStatus::Unavailable, cover.tiltState().status());
    ASSERT_TRUE(events.empty());
}

TEST(CoverTest, RequestsStopMotion)
{
    auto cover = liftAndTiltCover();
    ASSERT_EQ(Cover::Result::Ok, cover.requestClose());

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.requestStopMotion();

    ASSERT_EQ(Cover::Result::Ok, r);
    ASSERT_EQ(PositionStatus::Stopping, cover.liftState().status());
    ASSERT_EQ(CoverMotion::NotMoving, cover.liftState().motion());
    ASSERT_EQ(Position::fullyClosed(), cover.liftState().targetPosition());
    ASSERT_EQ(Position::fullyOpen(), cover.liftState().currentPosition());
    ASSERT_EQ(PositionStatus::Stopping, cover.tiltState().status());
    ASSERT_EQ(CoverMotion::NotMoving, cover.tiltState().motion());
    ASSERT_EQ(Position::fullyClosed(), cover.tiltState().targetPosition());
    ASSERT_EQ(Position::fullyOpen(), cover.tiltState().currentPosition());

    ASSERT_EQ(3u, events.size());
    ASSERT_STREQ(CoverLiftMotionChanged::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverLiftMotionChanged&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        ASSERT_EQ(cover.liftState().motion(), event.motion);
    }

    (void)events.pop();
    ASSERT_STREQ(CoverTiltMotionChanged::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverTiltMotionChanged&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        ASSERT_EQ(cover.tiltState().motion(), event.motion);
    }

    (void)events.pop();
    ASSERT_STREQ(CoverStopMotionRequested::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverStopMotionRequested&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
    }

    (void)events.pop();

    ASSERT_EQ(Cover::Result::NoChange, cover.requestStopMotion());
    ASSERT_TRUE(events.empty());
}

TEST(CoverTest, ReportsStopMotion)
{
    auto cover = liftAndTiltCover();
    ASSERT_EQ(Cover::Result::Ok, cover.requestClose());

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.reportStopMotion();

    ASSERT_EQ(Cover::Result::Ok, r);
    ASSERT_EQ(PositionStatus::Stopping, cover.liftState().status());
    ASSERT_EQ(CoverMotion::NotMoving, cover.liftState().motion());
    ASSERT_EQ(Position::fullyClosed(), cover.liftState().targetPosition());
    ASSERT_EQ(Position::fullyOpen(), cover.liftState().currentPosition());
    ASSERT_EQ(PositionStatus::Stopping, cover.tiltState().status());
    ASSERT_EQ(CoverMotion::NotMoving, cover.tiltState().motion());
    ASSERT_EQ(Position::fullyClosed(), cover.tiltState().targetPosition());
    ASSERT_EQ(Position::fullyOpen(), cover.tiltState().currentPosition());

    ASSERT_EQ(2u, events.size());
    ASSERT_STREQ(CoverLiftMotionChanged::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverLiftMotionChanged&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        ASSERT_EQ(cover.liftState().motion(), event.motion);
    }

    (void)events.pop();
    ASSERT_STREQ(CoverTiltMotionChanged::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverTiltMotionChanged&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        ASSERT_EQ(cover.tiltState().motion(), event.motion);
    }

    (void)events.pop();

    ASSERT_EQ(Cover::Result::NoChange, cover.reportStopMotion());
    ASSERT_TRUE(events.empty());
}

TEST(CoverTest, ReportsLiftToPosition)
{
    auto cover = liftCover();

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.reportLiftTo(Position::fullyClosed());

    ASSERT_EQ(Cover::Result::Ok, r);
    ASSERT_EQ(PositionStatus::Moving, cover.liftState().status());
    ASSERT_EQ(CoverMotion::Closing, cover.liftState().motion());
    ASSERT_EQ(Position::fullyClosed(), cover.liftState().targetPosition());
    ASSERT_EQ(Position::fullyOpen(), cover.liftState().currentPosition());

    ASSERT_EQ(2u, events.size());
    ASSERT_STREQ(CoverLiftMotionChanged::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverLiftMotionChanged&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        ASSERT_EQ(cover.liftState().motion(), event.motion);
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

    ASSERT_EQ(Cover::Result::NoChange, cover.reportLiftTo(*cover.liftState().targetPosition()));
    ASSERT_TRUE(events.empty());
}

TEST(CoverTest, ReportLiftToPositionFailsForTiltCover)
{
    auto cover = tiltCover();

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.reportLiftTo(Position::fullyClosed());

    ASSERT_EQ(Cover::Result::NotSupported, r);
    ASSERT_EQ(PositionStatus::Unavailable, cover.liftState().status());
    ASSERT_TRUE(events.empty());
}

TEST(CoverTest, ReportsTiltToPosition)
{
    auto cover = tiltCover();

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.reportTiltTo(Position::fullyClosed());

    ASSERT_EQ(Cover::Result::Ok, r);
    ASSERT_EQ(PositionStatus::Moving, cover.tiltState().status());
    ASSERT_EQ(CoverMotion::Closing, cover.tiltState().motion());
    ASSERT_EQ(Position::fullyClosed(), cover.tiltState().targetPosition());
    ASSERT_EQ(Position::fullyOpen(), cover.tiltState().currentPosition());

    ASSERT_EQ(2u, events.size());
    ASSERT_STREQ(CoverTiltMotionChanged::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverTiltMotionChanged&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        ASSERT_EQ(cover.tiltState().motion(), event.motion);
    }

    (void)events.pop();
    ASSERT_STREQ(CoverTiltTargetPositionChanged::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverTiltTargetPositionChanged&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        ASSERT_EQ(cover.tiltState().targetPosition(), event.position);
    }

    (void)events.pop();

    ASSERT_EQ(Cover::Result::NoChange, cover.reportTiltTo(*cover.tiltState().targetPosition()));
    ASSERT_TRUE(events.empty());
}

TEST(CoverTest, ReportTiltToPositionFailsForLiftCover)
{
    auto cover = liftCover();

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.reportTiltTo(Position::fullyClosed());

    ASSERT_EQ(Cover::Result::NotSupported, r);
    ASSERT_EQ(PositionStatus::Unavailable, cover.tiltState().status());
    ASSERT_TRUE(events.empty());
}

TEST(CoverTest, SyncesLiftPosition)
{
    auto cover = liftCover();

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.reportLiftPosition(Position::fullyClosed());

    ASSERT_EQ(Cover::Result::Ok, r);
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

    ASSERT_EQ(Cover::Result::NoChange, cover.reportLiftPosition(*cover.liftState().currentPosition()));
    ASSERT_TRUE(events.empty());
}

TEST(CoverTest, ChangesLiftPositionAfterMoveStarted)
{
    auto cover = liftCover();
    ASSERT_EQ(Cover::Result::Ok, cover.reportLiftTo(Position::fullyClosed()));

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.reportLiftPosition(Position::fullyClosed());

    ASSERT_EQ(Cover::Result::Ok, r);
    ASSERT_EQ(PositionStatus::Idle, cover.liftState().status());
    ASSERT_EQ(CoverMotion::NotMoving, cover.liftState().motion());
    ASSERT_EQ(Position::fullyClosed(), cover.liftState().targetPosition());
    ASSERT_EQ(Position::fullyClosed(), cover.liftState().currentPosition());

    ASSERT_EQ(2u, events.size());
    ASSERT_STREQ(CoverLiftMotionChanged::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverLiftMotionChanged&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        ASSERT_EQ(cover.liftState().motion(), event.motion);
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
    auto cover = liftCover();
    ASSERT_EQ(Cover::Result::Ok, cover.requestLiftTo(Position::fullyClosed()));

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.reportLiftPosition(Position::fullyClosed());

    ASSERT_EQ(Cover::Result::Ok, r);
    ASSERT_EQ(PositionStatus::Idle, cover.liftState().status());
    ASSERT_EQ(CoverMotion::NotMoving, cover.liftState().motion());
    ASSERT_EQ(Position::fullyClosed(), cover.liftState().targetPosition());
    ASSERT_EQ(Position::fullyClosed(), cover.liftState().currentPosition());

    ASSERT_EQ(2u, events.size());
    ASSERT_STREQ(CoverLiftMotionChanged::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverLiftMotionChanged&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        ASSERT_EQ(cover.liftState().motion(), event.motion);
    }

    (void)events.pop();
    ASSERT_STREQ(CoverLiftCurrentPositionChanged::kEventName, events.peek()->eventName());

    auto& event = static_cast<const CoverLiftCurrentPositionChanged&>(*events.peek());

    ASSERT_EQ(cover.endpointId(), event.endpointId);
    ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
    ASSERT_EQ(*cover.liftState().currentPosition(), event.position);

    (void)events.pop();
}

TEST(CoverTest, ChangesLiftPositionBackToCurrentPosition)
{
    auto cover = liftCover();
    ASSERT_EQ(Cover::Result::Ok, cover.requestLiftTo(Position::fullyClosed()));

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.reportLiftPosition(*cover.liftState().currentPosition());

    ASSERT_EQ(Cover::Result::Ok, r);
    ASSERT_EQ(PositionStatus::Idle, cover.liftState().status());
    ASSERT_EQ(CoverMotion::NotMoving, cover.liftState().motion());
    ASSERT_EQ(cover.liftState().currentPosition(), cover.liftState().targetPosition());

    ASSERT_EQ(2u, events.size());
    ASSERT_STREQ(CoverLiftMotionChanged::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverLiftMotionChanged&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        ASSERT_EQ(cover.liftState().motion(), event.motion);
    }

    (void)events.pop();
    ASSERT_STREQ(CoverLiftTargetPositionChanged::kEventName, events.peek()->eventName());

    auto& event = static_cast<const CoverLiftTargetPositionChanged&>(*events.peek());

    ASSERT_EQ(cover.endpointId(), event.endpointId);
    ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
    ASSERT_EQ(*cover.liftState().targetPosition(), event.position);

    (void)events.pop();
}

TEST(CoverTest, SyncesTiltPosition)
{
    auto cover = tiltCover();

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.reportTiltPosition(Position::fullyClosed());

    ASSERT_EQ(Cover::Result::Ok, r);
    ASSERT_EQ(PositionStatus::Idle, cover.tiltState().status());
    ASSERT_EQ(CoverMotion::NotMoving, cover.tiltState().motion());
    ASSERT_EQ(Position::fullyClosed(), cover.tiltState().targetPosition());
    ASSERT_EQ(Position::fullyClosed(), cover.tiltState().currentPosition());

    ASSERT_EQ(2u, events.size());
    ASSERT_STREQ(CoverTiltTargetPositionChanged::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverTiltTargetPositionChanged&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        ASSERT_EQ(*cover.tiltState().targetPosition(), event.position);
    }

    (void)events.pop();
    ASSERT_STREQ(CoverTiltCurrentPositionChanged::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverTiltCurrentPositionChanged&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        ASSERT_EQ(*cover.tiltState().currentPosition(), event.position);
    }

    (void)events.pop();

    ASSERT_EQ(Cover::Result::NoChange, cover.reportTiltPosition(*cover.tiltState().currentPosition()));
    ASSERT_TRUE(events.empty());
}

TEST(CoverTest, ChangesTiltPositionAfterMoveStarted)
{
    auto cover = tiltCover();
    ASSERT_EQ(Cover::Result::Ok, cover.reportTiltTo(Position::fullyClosed()));

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.reportTiltPosition(Position::fullyClosed());

    ASSERT_EQ(Cover::Result::Ok, r);
    ASSERT_EQ(PositionStatus::Idle, cover.tiltState().status());
    ASSERT_EQ(CoverMotion::NotMoving, cover.tiltState().motion());
    ASSERT_EQ(Position::fullyClosed(), cover.tiltState().targetPosition());
    ASSERT_EQ(Position::fullyClosed(), cover.tiltState().currentPosition());

    ASSERT_EQ(2u, events.size());
    ASSERT_STREQ(CoverTiltMotionChanged::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverTiltMotionChanged&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        ASSERT_EQ(cover.tiltState().motion(), event.motion);
    }

    (void)events.pop();
    ASSERT_STREQ(CoverTiltCurrentPositionChanged::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverTiltCurrentPositionChanged&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        ASSERT_EQ(*cover.tiltState().currentPosition(), event.position);
    }

    (void)events.pop();
}

TEST(CoverTest, ChangesTiltPositionAfterMoveRequest)
{
    auto cover = tiltCover();
    ASSERT_EQ(Cover::Result::Ok, cover.requestTiltTo(Position::fullyClosed()));

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.reportTiltPosition(Position::fullyClosed());

    ASSERT_EQ(Cover::Result::Ok, r);
    ASSERT_EQ(PositionStatus::Idle, cover.tiltState().status());
    ASSERT_EQ(CoverMotion::NotMoving, cover.tiltState().motion());
    ASSERT_EQ(Position::fullyClosed(), cover.tiltState().targetPosition());
    ASSERT_EQ(Position::fullyClosed(), cover.tiltState().currentPosition());

    ASSERT_EQ(2u, events.size());
    ASSERT_STREQ(CoverTiltMotionChanged::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverTiltMotionChanged&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        ASSERT_EQ(cover.tiltState().motion(), event.motion);
    }

    (void)events.pop();
    ASSERT_STREQ(CoverTiltCurrentPositionChanged::kEventName, events.peek()->eventName());

    auto& event = static_cast<const CoverTiltCurrentPositionChanged&>(*events.peek());

    ASSERT_EQ(cover.endpointId(), event.endpointId);
    ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
    ASSERT_EQ(*cover.tiltState().currentPosition(), event.position);

    (void)events.pop();
}

TEST(CoverTest, ChangesTiltPositionBackToCurrentPosition)
{
    auto cover = tiltCover();
    ASSERT_EQ(Cover::Result::Ok, cover.requestTiltTo(Position::fullyClosed()));

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.reportTiltPosition(*cover.liftState().currentPosition());

    ASSERT_EQ(Cover::Result::Ok, r);
    ASSERT_EQ(PositionStatus::Idle, cover.tiltState().status());
    ASSERT_EQ(CoverMotion::NotMoving, cover.tiltState().motion());
    ASSERT_EQ(cover.tiltState().currentPosition(), cover.tiltState().targetPosition());

    ASSERT_EQ(2u, events.size());
    ASSERT_STREQ(CoverTiltMotionChanged::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const CoverTiltMotionChanged&>(*events.peek());

        ASSERT_EQ(cover.endpointId(), event.endpointId);
        ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
        ASSERT_EQ(cover.tiltState().motion(), event.motion);
    }

    (void)events.pop();
    ASSERT_STREQ(CoverTiltTargetPositionChanged::kEventName, events.peek()->eventName());

    auto& event = static_cast<const CoverTiltTargetPositionChanged&>(*events.peek());

    ASSERT_EQ(cover.endpointId(), event.endpointId);
    ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);
    ASSERT_EQ(*cover.tiltState().currentPosition(), event.position);

    (void)events.pop();
}

TEST(CoverTest, ReportsUnreachableError)
{
    auto cover = liftCover();

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.reportError(Cover::Error::Unreachable);

    ASSERT_EQ(Cover::Result::Ok, r);
    ASSERT_FALSE(cover.isReachable());

    ASSERT_EQ(1u, events.size());
    ASSERT_STREQ(CoverMarkedAsUnreachable::kEventName, events.peek()->eventName());

    auto& event = static_cast<const CoverMarkedAsUnreachable&>(*events.peek());

    ASSERT_EQ(cover.endpointId(), event.endpointId);
    ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);

    (void)events.pop();

    ASSERT_EQ(Cover::Result::NoChange, cover.reportError(Cover::Error::Unreachable));
    ASSERT_TRUE(events.empty());
}

TEST(CoverTest, ReportsUnreachableErrorAfterMoveRequest)
{
    auto cover = liftAndTiltCover();
    ASSERT_EQ(Cover::Result::Ok, cover.requestClose());

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.reportError(Cover::Error::Unreachable);

    ASSERT_EQ(Cover::Result::Ok, r);
    ASSERT_FALSE(cover.isReachable());
    ASSERT_EQ(PositionStatus::Idle, cover.liftState().status());
    ASSERT_EQ(CoverMotion::NotMoving, cover.liftState().motion());
    ASSERT_EQ(Position::fullyOpen(), cover.liftState().targetPosition());
    ASSERT_EQ(Position::fullyOpen(), cover.liftState().currentPosition());
    ASSERT_EQ(PositionStatus::Idle, cover.tiltState().status());
    ASSERT_EQ(CoverMotion::NotMoving, cover.tiltState().motion());
    ASSERT_EQ(Position::fullyOpen(), cover.tiltState().targetPosition());
    ASSERT_EQ(Position::fullyOpen(), cover.tiltState().currentPosition());

    ASSERT_EQ(5u, events.size());
    ASSERT_STREQ(CoverMarkedAsUnreachable::kEventName, events.peek()->eventName());

    auto& event = static_cast<const CoverMarkedAsUnreachable&>(*events.peek());

    ASSERT_EQ(cover.endpointId(), event.endpointId);
    ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);

    (void)events.pop();
    ASSERT_STREQ(CoverLiftMotionChanged::kEventName, events.peek()->eventName());

    (void)events.pop();
    ASSERT_STREQ(CoverLiftTargetPositionChanged::kEventName, events.peek()->eventName());

    (void)events.pop();
    ASSERT_STREQ(CoverTiltMotionChanged::kEventName, events.peek()->eventName());

    (void)events.pop();
    ASSERT_STREQ(CoverTiltTargetPositionChanged::kEventName, events.peek()->eventName());

    (void)events.pop();

    ASSERT_EQ(Cover::Result::NoChange, cover.reportError(Cover::Error::Unknown));
    ASSERT_TRUE(events.empty());
}

TEST(CoverTest, ReportsCoverIsReachable)
{
    auto cover = liftCover();

    ASSERT_EQ(Cover::Result::Ok, cover.reportError(Cover::Error::Unreachable));
    ASSERT_FALSE(cover.isReachable());

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.reportReachable();

    ASSERT_EQ(Cover::Result::Ok, r);
    ASSERT_TRUE(cover.isReachable());

    ASSERT_EQ(1u, events.size());
    ASSERT_STREQ(CoverMarkedAsReachable::kEventName, events.peek()->eventName());

    auto& event = static_cast<const CoverMarkedAsReachable&>(*events.peek());

    ASSERT_EQ(cover.endpointId(), event.endpointId);
    ASSERT_EQ(cover.mobilusDeviceId(), event.mobilusDeviceId);

    (void)events.pop();

    ASSERT_EQ(Cover::Result::NoChange, cover.reportReachable());
    ASSERT_TRUE(events.empty());
}

TEST(CoverTest, ReportsErrorAfterMoveRequest)
{
    auto cover = liftAndTiltCover();
    ASSERT_EQ(Cover::Result::Ok, cover.requestClose());

    auto& events = DomainEventQueue::instance();
    events.clear();

    auto r = cover.reportError(Cover::Error::Unknown);

    ASSERT_EQ(Cover::Result::Ok, r);
    ASSERT_EQ(PositionStatus::Idle, cover.liftState().status());
    ASSERT_EQ(CoverMotion::NotMoving, cover.liftState().motion());
    ASSERT_EQ(Position::fullyOpen(), cover.liftState().targetPosition());
    ASSERT_EQ(Position::fullyOpen(), cover.liftState().currentPosition());
    ASSERT_EQ(PositionStatus::Idle, cover.tiltState().status());
    ASSERT_EQ(CoverMotion::NotMoving, cover.tiltState().motion());
    ASSERT_EQ(Position::fullyOpen(), cover.tiltState().targetPosition());
    ASSERT_EQ(Position::fullyOpen(), cover.tiltState().currentPosition());

    ASSERT_EQ(4u, events.size());

    ASSERT_STREQ(CoverLiftMotionChanged::kEventName, events.peek()->eventName());
    (void)events.pop();

    ASSERT_STREQ(CoverLiftTargetPositionChanged::kEventName, events.peek()->eventName());
    (void)events.pop();

    ASSERT_STREQ(CoverTiltMotionChanged::kEventName, events.peek()->eventName());
    (void)events.pop();

    ASSERT_STREQ(CoverTiltTargetPositionChanged::kEventName, events.peek()->eventName());
    (void)events.pop();

    ASSERT_EQ(Cover::Result::NoChange, cover.reportError(Cover::Error::Unknown));
    ASSERT_TRUE(events.empty());
}

TEST(CoverTest, ReportsRemoved)
{
    auto cover = liftCover();
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
