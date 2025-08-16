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
using namespace mmbridge::tests::driven_adapters::persistence::in_memory;
using namespace mmbridge::application::model;
using namespace mmbridge::application::model::window_covering;
using mmbridge::common::logging::Logger;
using mmbridge::driving_adapters::mobilus::cover::MobilusCoverInitHandler;
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

    EXPECT_TRUE(handler.supports(MobilusDeviceType::Senso));
    EXPECT_TRUE(handler.supports(MobilusDeviceType::Cosmo));
    EXPECT_TRUE(handler.supports(MobilusDeviceType::Cmr));
    EXPECT_FALSE(handler.supports(MobilusDeviceType::Cgr));
    EXPECT_FALSE(handler.supports(MobilusDeviceType::Switch));
    EXPECT_FALSE(handler.supports(MobilusDeviceType::SwitchNp));
    EXPECT_FALSE(handler.supports(MobilusDeviceType::CosmoCzr));
    EXPECT_FALSE(handler.supports(MobilusDeviceType::CosmoMzr));
    EXPECT_FALSE(handler.supports(MobilusDeviceType::SensoZ));
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

    handler.initDevice(device, event);

    auto cover = coverRepository.find(1u);
    EXPECT_TRUE(cover.has_value());
    EXPECT_TRUE(cover->isReachable());
    EXPECT_EQ(device.id(), cover->mobilusDeviceId());
    EXPECT_EQ(device.name(), cover->name());
    EXPECT_EQ(CoverMotion::NotMoving, cover->operationalStatus().lift());
    EXPECT_EQ(CoverMotion::NotMoving, cover->operationalStatus().tilt());
    EXPECT_EQ(PositionStatus::Idle, cover->liftState().status());
    EXPECT_EQ(CoverMotion::NotMoving, cover->liftState().motion());
    EXPECT_TRUE(cover->liftState().targetPosition().has_value());
    EXPECT_EQ(GetParam().expectedLiftPosition, *cover->liftState().targetPosition());
    EXPECT_TRUE(cover->liftState().currentPosition().has_value());
    EXPECT_EQ(GetParam().expectedLiftPosition, *cover->liftState().currentPosition());
    EXPECT_EQ(GetParam().expectedSpec, cover->specification());
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
