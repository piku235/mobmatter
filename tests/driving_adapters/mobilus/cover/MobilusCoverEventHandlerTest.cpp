#include "driving_adapters/mobilus/cover/MobilusCoverEventHandler.h"
#include "application/model/window_covering/CoverMotion.h"
#include "application/model/window_covering/CoverOperationalStatus.h"
#include "application/model/window_covering/CoverSpecification.h"
#include "application/model/window_covering/Position.h"
#include "application/model/window_covering/PositionStatus.h"
#include "common/logging/Logger.h"
#include "driven_adapters/persistence/in_memory/InMemoryCoverRepository.h"
#include "jungi/mobilus_gtw_client/EventNumber.h"
#include "jungi/mobilus_gtw_client/proto/Event.pb.h"

#include <gtest/gtest.h>

#include <string>

using namespace jungi::mobilus_gtw_client;
using namespace mmbridge::application::model;
using namespace mmbridge::application::model::window_covering;
using mmbridge::common::logging::Logger;
using mmbridge::driving_adapters::mobilus::cover::MobilusCoverEventHandler;
using mmbridge::tests::driven_adapters::persistence::in_memory::InMemoryCoverRepository;

namespace {

auto coverStub()
{
    return Cover::add(1, 2, "foo", PositionState::at(Position::fullyOpen()), CoverSpecification::Senso());
}

}

TEST(MobilusCoverEventHandlerTest, HandlesStartedCoverLift)
{
    InMemoryCoverRepository coverRepository;
    MobilusCoverEventHandler handler(coverRepository, Logger::noop());
    coverRepository.save(coverStub());

    proto::Event event;
    event.set_device_id(2);
    event.set_value("0%");
    event.set_event_number(EventNumber::Sent);

    handler.handle(event);
    auto cover = coverRepository.find(1u);

    ASSERT_TRUE(cover.has_value());
    ASSERT_EQ(CoverMotion::Closing, cover->operationalStatus().lift());
    ASSERT_EQ(CoverMotion::NotMoving, cover->operationalStatus().tilt());
    ASSERT_EQ(PositionStatus::Moving, cover->liftState().status());
    ASSERT_EQ(CoverMotion::Closing, cover->liftState().motion());
    ASSERT_EQ(Position::fullyClosed(), *cover->liftState().targetPosition());
    ASSERT_EQ(Position::fullyOpen(), *cover->liftState().currentPosition());
}

TEST(MobilusCoverEventHandlerTest, HandlesCoverReachedPosition)
{
    InMemoryCoverRepository coverRepository;
    MobilusCoverEventHandler handler(coverRepository, Logger::noop());

    {
        auto cover = coverStub();
        cover.startLiftTo(Position::fullyClosed());

        coverRepository.save(cover);
    }

    proto::Event event;
    event.set_device_id(2);
    event.set_value("0%");
    event.set_event_number(EventNumber::Reached);

    handler.handle(event);
    auto cover = coverRepository.find(1u);

    ASSERT_TRUE(cover.has_value());
    ASSERT_EQ(CoverMotion::NotMoving, cover->operationalStatus().lift());
    ASSERT_EQ(CoverMotion::NotMoving, cover->operationalStatus().tilt());
    ASSERT_EQ(PositionStatus::Idle, cover->liftState().status());
    ASSERT_EQ(CoverMotion::NotMoving, cover->liftState().motion());
    ASSERT_EQ(Position::fullyClosed(), *cover->liftState().targetPosition());
    ASSERT_EQ(Position::fullyClosed(), *cover->liftState().currentPosition());
}

TEST(MobilusCoverEventHandlerTest, HandlesStop)
{
    InMemoryCoverRepository coverRepository;
    MobilusCoverEventHandler handler(coverRepository, Logger::noop());

    {
        auto cover = coverStub();
        cover.startLiftTo(Position::fullyClosed());

        coverRepository.save(cover);
    }

    proto::Event event;
    event.set_device_id(2);
    event.set_value("STOP");
    event.set_event_number(EventNumber::Sent);

    handler.handle(event);
    auto cover = coverRepository.find(1u);

    ASSERT_TRUE(cover.has_value());
    ASSERT_EQ(CoverMotion::Closing, cover->operationalStatus().lift());
    ASSERT_EQ(CoverMotion::NotMoving, cover->operationalStatus().tilt());
    ASSERT_EQ(PositionStatus::Stopping, cover->liftState().status());
    ASSERT_EQ(CoverMotion::Closing, cover->liftState().motion());
    ASSERT_EQ(Position::fullyClosed(), *cover->liftState().targetPosition());
    ASSERT_EQ(Position::fullyOpen(), *cover->liftState().currentPosition());
}

TEST(MobilusCoverEventHandlerTest, HandlesCoverFailure)
{
    InMemoryCoverRepository coverRepository;
    MobilusCoverEventHandler handler(coverRepository, Logger::noop());

    {
        auto cover = coverStub();
        cover.startLiftTo(Position::fullyClosed());

        coverRepository.save(cover);
    }

    proto::Event event;
    event.set_device_id(2);
    event.set_value("UNKNOWN");
    event.set_event_number(EventNumber::Error);

    handler.handle(event);
    auto cover = coverRepository.find(1u);

    ASSERT_TRUE(cover.has_value());
    ASSERT_EQ(CoverMotion::NotMoving, cover->operationalStatus().lift());
    ASSERT_EQ(CoverMotion::NotMoving, cover->operationalStatus().tilt());
    ASSERT_EQ(PositionStatus::Idle, cover->liftState().status());
    ASSERT_EQ(CoverMotion::NotMoving, cover->liftState().motion());
    ASSERT_EQ(Position::fullyOpen(), *cover->liftState().targetPosition());
    ASSERT_EQ(Position::fullyOpen(), *cover->liftState().currentPosition());
}

TEST(MobilusCoverEventHandlerTest, HandlesNoConnection)
{
    InMemoryCoverRepository coverRepository;
    MobilusCoverEventHandler handler(coverRepository, Logger::noop());

    {
        auto cover = coverStub();
        cover.startLiftTo(Position::fullyClosed());

        coverRepository.save(cover);
    }

    proto::Event event;
    event.set_device_id(2);
    event.set_value("NO_CONNECTION");
    event.set_event_number(EventNumber::Error);

    handler.handle(event);
    auto cover = coverRepository.find(1u);

    ASSERT_TRUE(cover.has_value());
    ASSERT_FALSE(cover->isReachable());
    ASSERT_EQ(CoverMotion::NotMoving, cover->operationalStatus().lift());
    ASSERT_EQ(CoverMotion::NotMoving, cover->operationalStatus().tilt());
    ASSERT_EQ(PositionStatus::Idle, cover->liftState().status());
    ASSERT_EQ(CoverMotion::NotMoving, cover->liftState().motion());
    ASSERT_EQ(Position::fullyOpen(), *cover->liftState().targetPosition());
    ASSERT_EQ(Position::fullyOpen(), *cover->liftState().currentPosition());
}

TEST(MobilusCoverEventHandlerTest, HandlesRemovedCover)
{
    InMemoryCoverRepository coverRepository;
    MobilusCoverEventHandler handler(coverRepository, Logger::noop());

    {
        auto cover = coverStub();
        cover.startLiftTo(Position::fullyClosed());

        coverRepository.save(cover);
    }

    proto::Event event;
    event.set_device_id(2);
    event.set_value("REMOVE");
    event.set_event_number(EventNumber::Device);

    handler.handle(event);
    auto cover = coverRepository.find(1u);

    ASSERT_FALSE(cover.has_value());
}

TEST(MobilusCoverEventHandlerTest, IgnoresInvalidSentPosition)
{
    InMemoryCoverRepository coverRepository;
    MobilusCoverEventHandler handler(coverRepository, Logger::noop());
    coverRepository.save(coverStub());

    proto::Event event;
    event.set_device_id(2);
    event.set_value("INVALID");
    event.set_event_number(EventNumber::Sent);

    handler.handle(event);
    auto cover = coverRepository.find(1u);

    ASSERT_TRUE(cover.has_value());
    ASSERT_EQ(CoverMotion::NotMoving, cover->operationalStatus().lift());
    ASSERT_EQ(CoverMotion::NotMoving, cover->operationalStatus().tilt());
    ASSERT_EQ(PositionStatus::Idle, cover->liftState().status());
    ASSERT_EQ(CoverMotion::NotMoving, cover->liftState().motion());
    ASSERT_EQ(Position::fullyOpen(), *cover->liftState().targetPosition());
    ASSERT_EQ(Position::fullyOpen(), *cover->liftState().currentPosition());
}

TEST(MobilusCoverEventHandlerTest, IgnoresInvalidReachedPosition)
{
    InMemoryCoverRepository coverRepository;
    MobilusCoverEventHandler handler(coverRepository, Logger::noop());
    coverRepository.save(coverStub());

    proto::Event event;
    event.set_device_id(2);
    event.set_value("INVALID");
    event.set_event_number(EventNumber::Reached);

    handler.handle(event);
    auto cover = coverRepository.find(1u);

    ASSERT_TRUE(cover.has_value());
    ASSERT_EQ(CoverMotion::NotMoving, cover->operationalStatus().lift());
    ASSERT_EQ(CoverMotion::NotMoving, cover->operationalStatus().tilt());
    ASSERT_EQ(PositionStatus::Idle, cover->liftState().status());
    ASSERT_EQ(CoverMotion::NotMoving, cover->liftState().motion());
    ASSERT_EQ(Position::fullyOpen(), *cover->liftState().targetPosition());
    ASSERT_EQ(Position::fullyOpen(), *cover->liftState().currentPosition());
}

TEST(MobilusCoverEventHandlerTest, IgnoresUnsupportedEvent)
{
    InMemoryCoverRepository coverRepository;
    MobilusCoverEventHandler handler(coverRepository, Logger::noop());

    proto::Event event;
    event.set_value("ADD");
    event.set_event_number(EventNumber::User);

    handler.handle(event);
}
