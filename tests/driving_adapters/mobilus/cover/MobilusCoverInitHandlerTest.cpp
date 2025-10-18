#include "driving_adapters/mobilus/cover/MobilusCoverInitHandler.h"
#include "application/model/MobilusDeviceType.h"
#include "application/model/window_covering/CoverMotion.h"
#include "application/model/window_covering/CoverOperationalStatus.h"
#include "application/model/window_covering/CoverSpecification.h"
#include "application/model/window_covering/Position.h"
#include "application/model/window_covering/PositionStatus.h"
#include "common/logging/Logger.h"
#include "driven_adapters/persistence/in_memory/InMemoryCoverRepository.h"
#include "driven_adapters/persistence/in_memory/InMemoryEndpointIdGenerator.h"
#include "jungi/mobilus_gtw_client/EventNumber.h"
#include "jungi/mobilus_gtw_client/proto/Device.pb.h"
#include "jungi/mobilus_gtw_client/proto/Event.pb.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <string>

using namespace jungi::mobilus_gtw_client;
using namespace mobmatter::tests::driven_adapters::persistence::in_memory;
using namespace mobmatter::application::model;
using namespace mobmatter::application::model::window_covering;
using mobmatter::common::logging::Logger;
using mobmatter::driving_adapters::mobilus::cover::MobilusCoverInitHandler;
using testing::TestWithParam;
using testing::Values;

struct CoverExpectation {
    Position expectedLiftPosition;
    CoverSpecification expectedSpec;
    MobilusDeviceType deviceType;
    std::string eventValue;
    uint8_t eventNumber;
};

class MobilusCoverInitHandlerTest : public TestWithParam<CoverExpectation> {
};

TEST(MobilusCoverInitHandlerTest, SupportsAndDoesNotSupportDeviceType)
{
    InMemoryCoverRepository coverRepository;
    InMemoryEndpointIdGenerator endpointIdGenerator(1u);
    MobilusCoverInitHandler handler(coverRepository, endpointIdGenerator, Logger::noop());

    ASSERT_TRUE(handler.supports(MobilusDeviceType::Senso));
    ASSERT_TRUE(handler.supports(MobilusDeviceType::Cosmo));
    ASSERT_TRUE(handler.supports(MobilusDeviceType::Cmr));
    ASSERT_FALSE(handler.supports(MobilusDeviceType::Cgr));
    ASSERT_FALSE(handler.supports(MobilusDeviceType::Switch));
    ASSERT_FALSE(handler.supports(MobilusDeviceType::SwitchNp));
    ASSERT_FALSE(handler.supports(MobilusDeviceType::CosmoCzr));
    ASSERT_FALSE(handler.supports(MobilusDeviceType::CosmoMzr));
    ASSERT_FALSE(handler.supports(MobilusDeviceType::SensoZ));
}

TEST_P(MobilusCoverInitHandlerTest, InitiatesCovers)
{
    InMemoryCoverRepository coverRepository;
    InMemoryEndpointIdGenerator endpointIdGenerator(1u);
    MobilusCoverInitHandler handler(coverRepository, endpointIdGenerator, Logger::noop());

    proto::Device device;
    device.set_id(1);
    device.set_name("foo");
    device.set_type(static_cast<int>(GetParam().deviceType));

    proto::Event event;
    event.set_device_id(device.id());
    event.set_value(GetParam().eventValue);
    event.set_event_number(GetParam().eventNumber);

    handler.handle(device, event);

    auto cover = coverRepository.find(1u);
    ASSERT_TRUE(cover.has_value());
    ASSERT_TRUE(cover->isReachable());
    ASSERT_EQ(device.id(), cover->mobilusDeviceId());
    ASSERT_EQ(device.name(), cover->name());
    ASSERT_EQ(CoverMotion::NotMoving, cover->operationalStatus().lift());
    ASSERT_EQ(CoverMotion::NotMoving, cover->operationalStatus().tilt());
    ASSERT_EQ(PositionStatus::Idle, cover->liftState().status());
    ASSERT_EQ(CoverMotion::NotMoving, cover->liftState().motion());
    ASSERT_TRUE(cover->liftState().targetPosition().has_value());
    ASSERT_EQ(GetParam().expectedLiftPosition, *cover->liftState().targetPosition());
    ASSERT_TRUE(cover->liftState().currentPosition().has_value());
    ASSERT_EQ(GetParam().expectedLiftPosition, *cover->liftState().currentPosition());
    ASSERT_EQ(GetParam().expectedSpec, cover->specification());
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(PossibleDevices, MobilusCoverInitHandlerTest, Values(
    CoverExpectation { Position::fullyOpen(), CoverSpecification::Senso(), MobilusDeviceType::Senso, "100%", EventNumber::Reached },
    CoverExpectation { Position::fullyOpen(), CoverSpecification::Senso(), MobilusDeviceType::Senso, "UP", EventNumber::Reached },
    CoverExpectation { Position::fullyClosed(), CoverSpecification::Cosmo(), MobilusDeviceType::Cosmo, "0%", EventNumber::Sent },
    CoverExpectation { Position::fullyClosed(), CoverSpecification::Cosmo(), MobilusDeviceType::Cosmo, "DOWN", EventNumber::Sent },
    CoverExpectation { Position::fullyClosed(), CoverSpecification::Cmr(), MobilusDeviceType::Cmr, "UNKNOWN", EventNumber::Error },
    CoverExpectation { Position::fullyClosed(), CoverSpecification::Cmr(), MobilusDeviceType::Cmr, "12", EventNumber::Reached }
));
// clang-format on
