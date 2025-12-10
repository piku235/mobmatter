#include "driving_adapters/mobilus/device_handlers/MobilusCoverHandler.h"
#include "application/model/MobilusDeviceType.h"
#include "application/model/window_covering/Cover.h"
#include "application/model/window_covering/CoverMotion.h"
#include "application/model/window_covering/CoverOperationalStatus.h"
#include "application/model/window_covering/CoverSpecification.h"
#include "application/model/window_covering/Position.h"
#include "application/model/window_covering/PositionState.h"
#include "application/model/window_covering/PositionStatus.h"
#include "common/logging/Logger.h"
#include "driven_adapters/persistence/in_memory/InMemoryCoverRepository.h"
#include "driven_adapters/persistence/in_memory/InMemoryEndpointIdGenerator.h"
#include "driving_adapters/mobilus/DeviceStateMap.h"
#include "driving_adapters/mobilus/MobilusDeviceEventHandler.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <jungi/mobgtw/EventNumber.h>
#include <jungi/mobgtw/proto/Device.pb.h>
#include <jungi/mobgtw/proto/Event.pb.h>

#include <string>

using namespace jungi::mobgtw;
using namespace mobmatter::tests::driven_adapters::persistence::in_memory;
using namespace mobmatter::application::model;
using namespace mobmatter::application::model::window_covering;
using namespace mobmatter::driving_adapters::mobilus;
using namespace mobmatter::driving_adapters::mobilus::device_handlers;
using mobmatter::common::logging::Logger;
using testing::TestWithParam;
using testing::Values;

namespace {

constexpr MobilusDeviceId kMobilusDeviceId = 2;

struct CoverExpectation {
    Position expectedLiftPosition;
    CoverSpecification expectedSpec;
    MobilusDeviceType deviceType;
    std::string eventValue;
    uint8_t eventNumber;
};

auto coverStub()
{
    return Cover::add(1, kMobilusDeviceId, "Foo", PositionState::at(Position::fullyOpen()), CoverSpecification::Senso());
}

}

class MobilusCoverHandlerInitTest : public TestWithParam<CoverExpectation> { };

TEST(MobilusCoverHandlerTest, EventIsNotSupported)
{
    InMemoryCoverRepository coverRepository;
    InMemoryEndpointIdGenerator endpointIdGenerator(1u);
    MobilusCoverHandler handler(coverRepository, endpointIdGenerator, Logger::noop());

    proto::Event event;
    event.set_device_id(kMobilusDeviceId);
    event.set_value("OFF");
    event.set_event_number(EventNumber::Reached);

    ASSERT_EQ(MobilusDeviceEventHandler::Result::Unmatched, handler.handle(event));
}

TEST(MobilusCoverHandlerTest, DoesNotSyncUnsupportedDevice)
{
    InMemoryCoverRepository coverRepository;
    InMemoryEndpointIdGenerator endpointIdGenerator(1u);
    MobilusCoverHandler handler(coverRepository, endpointIdGenerator, Logger::noop());

    DeviceStateMap devices;
    devices[kMobilusDeviceId].device.set_id(kMobilusDeviceId);
    devices[kMobilusDeviceId].device.set_name("foo");
    devices[kMobilusDeviceId].device.set_type(static_cast<int>(MobilusDeviceType::Switch));

    devices[kMobilusDeviceId].lastEvent.set_device_id(kMobilusDeviceId);
    devices[kMobilusDeviceId].lastEvent.set_value("OFF");
    devices[kMobilusDeviceId].lastEvent.set_event_number(EventNumber::Reached);

    handler.sync(devices);

    ASSERT_TRUE(coverRepository.all().empty());
}

TEST_P(MobilusCoverHandlerInitTest, SynchronizesNewCover)
{
    InMemoryCoverRepository coverRepository;
    InMemoryEndpointIdGenerator endpointIdGenerator(1u);
    MobilusCoverHandler handler(coverRepository, endpointIdGenerator, Logger::noop());

    DeviceStateMap devices;
    devices[kMobilusDeviceId].device.set_id(kMobilusDeviceId);
    devices[kMobilusDeviceId].device.set_name("foo");
    devices[kMobilusDeviceId].device.set_type(static_cast<int>(GetParam().deviceType));

    devices[kMobilusDeviceId].lastEvent.set_device_id(kMobilusDeviceId);
    devices[kMobilusDeviceId].lastEvent.set_value(GetParam().eventValue);
    devices[kMobilusDeviceId].lastEvent.set_event_number(GetParam().eventNumber);

    handler.sync(devices);

    auto cover = coverRepository.find(1u);
    auto& device = devices[kMobilusDeviceId].device;

    ASSERT_TRUE(cover.has_value());
    ASSERT_TRUE(cover->isReachable());
    ASSERT_EQ(device.id(), cover->mobilusDeviceId());
    ASSERT_EQ(device.name(), cover->name());
    ASSERT_EQ(CoverMotion::NotMoving, cover->operationalStatus().lift());
    ASSERT_EQ(CoverMotion::NotMoving, cover->operationalStatus().tilt());
    ASSERT_EQ(PositionStatus::Idle, cover->liftState().status());
    ASSERT_EQ(CoverMotion::NotMoving, cover->liftState().motion());
    ASSERT_EQ(GetParam().expectedLiftPosition, cover->liftState().targetPosition());
    ASSERT_EQ(GetParam().expectedLiftPosition, cover->liftState().currentPosition());
    ASSERT_EQ(GetParam().expectedSpec, cover->specification());
}

TEST(MobilusCoverHandlerTest, RemovesNonExistingCover)
{
    InMemoryCoverRepository coverRepository;
    InMemoryEndpointIdGenerator endpointIdGenerator(1u);
    MobilusCoverHandler handler(coverRepository, endpointIdGenerator, Logger::noop());

    coverRepository.save(coverStub());
    ASSERT_TRUE(coverRepository.findOfMobilusDeviceId(kMobilusDeviceId).has_value());

    DeviceStateMap devices;
    handler.sync(devices);

    ASSERT_FALSE(coverRepository.findOfMobilusDeviceId(kMobilusDeviceId).has_value());
}

TEST(MobilusCoverHandlerTest, SynchronizesCoverCurrentPosition)
{
    InMemoryCoverRepository coverRepository;
    InMemoryEndpointIdGenerator endpointIdGenerator(1u);
    MobilusCoverHandler handler(coverRepository, endpointIdGenerator, Logger::noop());
    coverRepository.save(coverStub());

    DeviceStateMap devices;
    devices[kMobilusDeviceId].device.set_id(kMobilusDeviceId);
    devices[kMobilusDeviceId].device.set_name("Foo");
    devices[kMobilusDeviceId].device.set_type(static_cast<int>(MobilusDeviceType::Senso));

    devices[kMobilusDeviceId].lastEvent.set_device_id(kMobilusDeviceId);
    devices[kMobilusDeviceId].lastEvent.set_value("0%");
    devices[kMobilusDeviceId].lastEvent.set_event_number(EventNumber::Reached);

    handler.sync(devices);

    auto cover = coverRepository.find(1u);
    auto& device = devices[kMobilusDeviceId].device;

    ASSERT_TRUE(cover.has_value());
    ASSERT_EQ(device.id(), cover->mobilusDeviceId());
    ASSERT_EQ(device.name(), cover->name());
    ASSERT_EQ(CoverMotion::NotMoving, cover->operationalStatus().lift());
    ASSERT_EQ(CoverMotion::NotMoving, cover->operationalStatus().tilt());
    ASSERT_EQ(PositionStatus::Idle, cover->liftState().status());
    ASSERT_EQ(CoverMotion::NotMoving, cover->liftState().motion());
    ASSERT_EQ(Position::fullyClosed(), cover->liftState().targetPosition());
    ASSERT_EQ(Position::fullyClosed(), cover->liftState().currentPosition());
}

TEST(MobilusCoverHandlerTest, SynchronizesCoverTargetPosition)
{
    InMemoryCoverRepository coverRepository;
    InMemoryEndpointIdGenerator endpointIdGenerator(1u);
    MobilusCoverHandler handler(coverRepository, endpointIdGenerator, Logger::noop());
    coverRepository.save(coverStub());

    DeviceStateMap devices;
    devices[kMobilusDeviceId].device.set_id(kMobilusDeviceId);
    devices[kMobilusDeviceId].device.set_name("Foo");
    devices[kMobilusDeviceId].device.set_type(static_cast<int>(MobilusDeviceType::Senso));

    devices[kMobilusDeviceId].lastEvent.set_device_id(kMobilusDeviceId);
    devices[kMobilusDeviceId].lastEvent.set_value("0%");
    devices[kMobilusDeviceId].lastEvent.set_event_number(EventNumber::Sent);

    handler.sync(devices);

    auto cover = coverRepository.find(1u);
    auto& device = devices[kMobilusDeviceId].device;

    ASSERT_TRUE(cover.has_value());
    ASSERT_EQ(device.id(), cover->mobilusDeviceId());
    ASSERT_EQ(device.name(), cover->name());
    ASSERT_EQ(CoverMotion::Closing, cover->operationalStatus().lift());
    ASSERT_EQ(CoverMotion::NotMoving, cover->operationalStatus().tilt());
    ASSERT_EQ(PositionStatus::Moving, cover->liftState().status());
    ASSERT_EQ(CoverMotion::Closing, cover->liftState().motion());
    ASSERT_EQ(Position::fullyClosed(), cover->liftState().targetPosition());
    ASSERT_EQ(Position::fullyOpen(), cover->liftState().currentPosition());
}

TEST(MobilusCoverHandlerTest, SynchronizesCoverName)
{
    InMemoryCoverRepository coverRepository;
    InMemoryEndpointIdGenerator endpointIdGenerator(1u);
    MobilusCoverHandler handler(coverRepository, endpointIdGenerator, Logger::noop());
    coverRepository.save(coverStub());

    DeviceStateMap devices;
    devices[kMobilusDeviceId].device.set_id(kMobilusDeviceId);
    devices[kMobilusDeviceId].device.set_name("Bar");
    devices[kMobilusDeviceId].device.set_type(static_cast<int>(MobilusDeviceType::Senso));

    devices[kMobilusDeviceId].lastEvent.set_device_id(kMobilusDeviceId);
    devices[kMobilusDeviceId].lastEvent.set_value("100%");
    devices[kMobilusDeviceId].lastEvent.set_event_number(EventNumber::Reached);

    handler.sync(devices);

    auto cover = coverRepository.find(1u);
    auto& device = devices[kMobilusDeviceId].device;

    ASSERT_TRUE(cover.has_value());
    ASSERT_EQ(device.id(), cover->mobilusDeviceId());
    ASSERT_EQ(device.name(), cover->name());
    ASSERT_EQ(CoverMotion::NotMoving, cover->operationalStatus().lift());
    ASSERT_EQ(CoverMotion::NotMoving, cover->operationalStatus().tilt());
    ASSERT_EQ(PositionStatus::Idle, cover->liftState().status());
    ASSERT_EQ(CoverMotion::NotMoving, cover->liftState().motion());
    ASSERT_EQ(Position::fullyOpen(), cover->liftState().targetPosition());
    ASSERT_EQ(Position::fullyOpen(), cover->liftState().currentPosition());
}

TEST(MobilusCoverHandlerTest, SynchronizesMixChanges)
{
    InMemoryCoverRepository coverRepository;
    InMemoryEndpointIdGenerator endpointIdGenerator(1u);
    MobilusCoverHandler handler(coverRepository, endpointIdGenerator, Logger::noop());
    coverRepository.save(coverStub());

    DeviceStateMap devices;
    devices[kMobilusDeviceId].device.set_id(kMobilusDeviceId);
    devices[kMobilusDeviceId].device.set_name("Bar");
    devices[kMobilusDeviceId].device.set_type(static_cast<int>(MobilusDeviceType::Senso));

    devices[kMobilusDeviceId].lastEvent.set_device_id(kMobilusDeviceId);
    devices[kMobilusDeviceId].lastEvent.set_value("0%");
    devices[kMobilusDeviceId].lastEvent.set_event_number(EventNumber::Reached);

    handler.sync(devices);

    auto cover = coverRepository.find(1u);
    auto& device = devices[kMobilusDeviceId].device;

    ASSERT_TRUE(cover.has_value());
    ASSERT_EQ(device.id(), cover->mobilusDeviceId());
    ASSERT_EQ(device.name(), cover->name());
    ASSERT_EQ(CoverMotion::NotMoving, cover->operationalStatus().lift());
    ASSERT_EQ(CoverMotion::NotMoving, cover->operationalStatus().tilt());
    ASSERT_EQ(PositionStatus::Idle, cover->liftState().status());
    ASSERT_EQ(CoverMotion::NotMoving, cover->liftState().motion());
    ASSERT_EQ(Position::fullyClosed(), cover->liftState().targetPosition());
    ASSERT_EQ(Position::fullyClosed(), cover->liftState().currentPosition());
}

TEST(MobilusCoverHandlerTest, HandlesStartedCoverLiftEvent)
{
    InMemoryCoverRepository coverRepository;
    InMemoryEndpointIdGenerator endpointIdGenerator(1u);
    MobilusCoverHandler handler(coverRepository, endpointIdGenerator, Logger::noop());
    coverRepository.save(coverStub());

    proto::Event event;
    event.set_device_id(kMobilusDeviceId);
    event.set_value("0%");
    event.set_event_number(EventNumber::Sent);

    ASSERT_EQ(MobilusDeviceEventHandler::Result::Handled, handler.handle(event));
    auto cover = coverRepository.find(1u);

    ASSERT_TRUE(cover.has_value());
    ASSERT_EQ(CoverMotion::Closing, cover->operationalStatus().lift());
    ASSERT_EQ(CoverMotion::NotMoving, cover->operationalStatus().tilt());
    ASSERT_EQ(PositionStatus::Moving, cover->liftState().status());
    ASSERT_EQ(CoverMotion::Closing, cover->liftState().motion());
    ASSERT_EQ(Position::fullyClosed(), cover->liftState().targetPosition());
    ASSERT_EQ(Position::fullyOpen(), cover->liftState().currentPosition());
}

TEST(MobilusCoverHandlerTest, HandlesCoverReachedPositionEvent)
{
    InMemoryCoverRepository coverRepository;
    InMemoryEndpointIdGenerator endpointIdGenerator(1u);
    MobilusCoverHandler handler(coverRepository, endpointIdGenerator, Logger::noop());

    {
        auto cover = coverStub();
        cover.startLiftTo(Position::fullyClosed());

        coverRepository.save(cover);
    }

    proto::Event event;
    event.set_device_id(kMobilusDeviceId);
    event.set_value("0%");
    event.set_event_number(EventNumber::Reached);

    ASSERT_EQ(MobilusDeviceEventHandler::Result::Handled, handler.handle(event));
    auto cover = coverRepository.find(1u);

    ASSERT_TRUE(cover.has_value());
    ASSERT_EQ(CoverMotion::NotMoving, cover->operationalStatus().lift());
    ASSERT_EQ(CoverMotion::NotMoving, cover->operationalStatus().tilt());
    ASSERT_EQ(PositionStatus::Idle, cover->liftState().status());
    ASSERT_EQ(CoverMotion::NotMoving, cover->liftState().motion());
    ASSERT_EQ(Position::fullyClosed(), cover->liftState().targetPosition());
    ASSERT_EQ(Position::fullyClosed(), cover->liftState().currentPosition());
}

TEST(MobilusCoverHandlerTest, HandlesStopMotionEvent)
{
    InMemoryCoverRepository coverRepository;
    InMemoryEndpointIdGenerator endpointIdGenerator(1u);
    MobilusCoverHandler handler(coverRepository, endpointIdGenerator, Logger::noop());

    {
        auto cover = coverStub();
        cover.startLiftTo(Position::fullyClosed());

        coverRepository.save(cover);
    }

    proto::Event event;
    event.set_device_id(kMobilusDeviceId);
    event.set_value("STOP");
    event.set_event_number(EventNumber::Sent);

    ASSERT_EQ(MobilusDeviceEventHandler::Result::Handled, handler.handle(event));
    auto cover = coverRepository.find(1u);

    ASSERT_TRUE(cover.has_value());
    ASSERT_EQ(CoverMotion::Closing, cover->operationalStatus().lift());
    ASSERT_EQ(CoverMotion::NotMoving, cover->operationalStatus().tilt());
    ASSERT_EQ(PositionStatus::Stopping, cover->liftState().status());
    ASSERT_EQ(CoverMotion::Closing, cover->liftState().motion());
    ASSERT_EQ(Position::fullyClosed(), cover->liftState().targetPosition());
    ASSERT_EQ(Position::fullyOpen(), cover->liftState().currentPosition());
}

TEST(MobilusCoverHandlerTest, HandlesCoverFailureEvent)
{
    InMemoryCoverRepository coverRepository;
    InMemoryEndpointIdGenerator endpointIdGenerator(1u);
    MobilusCoverHandler handler(coverRepository, endpointIdGenerator, Logger::noop());

    {
        auto cover = coverStub();
        cover.startLiftTo(Position::fullyClosed());

        coverRepository.save(cover);
    }

    proto::Event event;
    event.set_device_id(kMobilusDeviceId);
    event.set_value("UNKNOWN");
    event.set_event_number(EventNumber::Error);

    ASSERT_EQ(MobilusDeviceEventHandler::Result::Handled, handler.handle(event));
    auto cover = coverRepository.find(1u);

    ASSERT_TRUE(cover.has_value());
    ASSERT_EQ(CoverMotion::NotMoving, cover->operationalStatus().lift());
    ASSERT_EQ(CoverMotion::NotMoving, cover->operationalStatus().tilt());
    ASSERT_EQ(PositionStatus::Idle, cover->liftState().status());
    ASSERT_EQ(CoverMotion::NotMoving, cover->liftState().motion());
    ASSERT_EQ(Position::fullyOpen(), cover->liftState().targetPosition());
    ASSERT_EQ(Position::fullyOpen(), cover->liftState().currentPosition());
}

TEST(MobilusCoverHandlerTest, HandlesNoConnectionEvent)
{
    InMemoryCoverRepository coverRepository;
    InMemoryEndpointIdGenerator endpointIdGenerator(1u);
    MobilusCoverHandler handler(coverRepository, endpointIdGenerator, Logger::noop());

    {
        auto cover = coverStub();
        cover.startLiftTo(Position::fullyClosed());

        coverRepository.save(cover);
    }

    proto::Event event;
    event.set_device_id(kMobilusDeviceId);
    event.set_value("NO_CONNECTION");
    event.set_event_number(EventNumber::Error);

    ASSERT_EQ(MobilusDeviceEventHandler::Result::Handled, handler.handle(event));
    auto cover = coverRepository.find(1u);

    ASSERT_TRUE(cover.has_value());
    ASSERT_FALSE(cover->isReachable());
    ASSERT_EQ(CoverMotion::NotMoving, cover->operationalStatus().lift());
    ASSERT_EQ(CoverMotion::NotMoving, cover->operationalStatus().tilt());
    ASSERT_EQ(PositionStatus::Idle, cover->liftState().status());
    ASSERT_EQ(CoverMotion::NotMoving, cover->liftState().motion());
    ASSERT_EQ(Position::fullyOpen(), cover->liftState().targetPosition());
    ASSERT_EQ(Position::fullyOpen(), cover->liftState().currentPosition());
}

TEST(MobilusCoverHandlerTest, IgnoresInvalidSentPosition)
{
    InMemoryCoverRepository coverRepository;
    InMemoryEndpointIdGenerator endpointIdGenerator(1u);
    MobilusCoverHandler handler(coverRepository, endpointIdGenerator, Logger::noop());
    coverRepository.save(coverStub());

    proto::Event event;
    event.set_device_id(kMobilusDeviceId);
    event.set_value("INVALID");
    event.set_event_number(EventNumber::Sent);

    ASSERT_EQ(MobilusDeviceEventHandler::Result::Handled, handler.handle(event));
    auto cover = coverRepository.find(1u);

    ASSERT_TRUE(cover.has_value());
    ASSERT_EQ(CoverMotion::NotMoving, cover->operationalStatus().lift());
    ASSERT_EQ(CoverMotion::NotMoving, cover->operationalStatus().tilt());
    ASSERT_EQ(PositionStatus::Idle, cover->liftState().status());
    ASSERT_EQ(CoverMotion::NotMoving, cover->liftState().motion());
    ASSERT_EQ(Position::fullyOpen(), cover->liftState().targetPosition());
    ASSERT_EQ(Position::fullyOpen(), cover->liftState().currentPosition());
}

TEST(MobilusCoverHandlerTest, IgnoresInvalidReachedPosition)
{
    InMemoryCoverRepository coverRepository;
    InMemoryEndpointIdGenerator endpointIdGenerator(1u);
    MobilusCoverHandler handler(coverRepository, endpointIdGenerator, Logger::noop());

    coverRepository.save(coverStub());

    proto::Event event;
    event.set_device_id(kMobilusDeviceId);
    event.set_value("INVALID");
    event.set_event_number(EventNumber::Reached);

    ASSERT_EQ(MobilusDeviceEventHandler::Result::Handled, handler.handle(event));
    auto cover = coverRepository.find(1u);

    ASSERT_TRUE(cover.has_value());
    ASSERT_EQ(CoverMotion::NotMoving, cover->operationalStatus().lift());
    ASSERT_EQ(CoverMotion::NotMoving, cover->operationalStatus().tilt());
    ASSERT_EQ(PositionStatus::Idle, cover->liftState().status());
    ASSERT_EQ(CoverMotion::NotMoving, cover->liftState().motion());
    ASSERT_EQ(Position::fullyOpen(), cover->liftState().targetPosition());
    ASSERT_EQ(Position::fullyOpen(), cover->liftState().currentPosition());
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(PossibleDevices, MobilusCoverHandlerInitTest, Values(
    CoverExpectation { Position::fullyOpen(), CoverSpecification::Senso(), MobilusDeviceType::Senso, "100%", EventNumber::Reached },
    CoverExpectation { Position::fullyOpen(), CoverSpecification::Senso(), MobilusDeviceType::Senso, "UP", EventNumber::Reached },
    CoverExpectation { Position::fullyClosed(), CoverSpecification::Cosmo(), MobilusDeviceType::Cosmo, "0%", EventNumber::Sent },
    CoverExpectation { Position::fullyClosed(), CoverSpecification::Cosmo(), MobilusDeviceType::Cosmo, "DOWN", EventNumber::Sent },
    CoverExpectation { Position::fullyClosed(), CoverSpecification::Cmr(), MobilusDeviceType::Cmr, "UNKNOWN", EventNumber::Error },
    CoverExpectation { Position::fullyClosed(), CoverSpecification::Cmr(), MobilusDeviceType::Cmr, "12", EventNumber::Reached }
));
// clang-format on
