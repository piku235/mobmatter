#include "driven_adapters/mobilus/MqttMobilusCoverControlService.h"
#include "application/model/window_covering/Position.h"
#include "common/logging/Logger.h"
#include "jungi/mobilus_gtw_client/EventNumber.h"
#include "jungi/mobilus_gtw_client/MessageType.h"
#include "jungi/mobilus_gtw_client/Platform.h"
#include "jungi/mobilus_gtw_client/ProtoUtils.h"
#include "jungi/mobilus_gtw_client/proto/CallEvents.pb.h"
#include "mobilus/MockMqttMobilusGtwClient.hpp"

#include <gtest/gtest.h>

#include <cstdint>
#include <tuple>

using namespace jungi::mobilus_gtw_client;
using mobmatter::application::model::Percent;
using mobmatter::application::model::window_covering::Position;
using mobmatter::common::logging::Logger;
using mobmatter::driven_adapters::mobilus::MqttMobilusCoverControlService;
using mobmatter::tests::mobilus::MockMqttMobilusGtwClient;
using testing::TestWithParam;
using testing::Values;
using ExpectedPosition = std::tuple<std::string, uint16_t>;

class MqttMobilusCoverControlServiceParamTest : public TestWithParam<ExpectedPosition> { };

TEST_P(MqttMobilusCoverControlServiceParamTest, LiftsCover)
{
    MockMqttMobilusGtwClient client;
    MqttMobilusCoverControlService coverControlService(client, Logger::noop());
    auto [expectedEventValue, position] = GetParam();

    coverControlService.liftCover(23, Position::open(*Percent::from100ths(position)));

    ASSERT_EQ(1, client.sentMessages().size());
    ASSERT_EQ(MessageType::CallEvents, ProtoUtils::messageTypeFor(*client.sentMessages()[0]));

    auto& callEvents = static_cast<const proto::CallEvents&>(*client.sentMessages()[0]);

    ASSERT_EQ(1, callEvents.events_size());
    ASSERT_EQ(23, callEvents.events(0).device_id());
    ASSERT_EQ(expectedEventValue, callEvents.events(0).value());
    ASSERT_EQ(EventNumber::Triggered, callEvents.events(0).event_number());
    ASSERT_EQ(Platform::Web, callEvents.events(0).platform());
    ASSERT_FALSE(callEvents.events(0).has_id());
    ASSERT_FALSE(callEvents.events(0).has_user());
    ASSERT_FALSE(callEvents.events(0).has_inserttime());
}

TEST(MqttMobilusCoverControlServiceTest, StopsCoverMotion)
{
    MockMqttMobilusGtwClient client;
    MqttMobilusCoverControlService coverControlService(client, Logger::noop());

    coverControlService.stopCoverMotion(23);

    ASSERT_EQ(1, client.sentMessages().size());
    ASSERT_EQ(MessageType::CallEvents, ProtoUtils::messageTypeFor(*client.sentMessages()[0]));

    auto& callEvents = static_cast<const proto::CallEvents&>(*client.sentMessages()[0]);

    ASSERT_EQ(1, callEvents.events_size());
    ASSERT_EQ(23, callEvents.events(0).device_id());
    ASSERT_EQ("STOP", callEvents.events(0).value());
    ASSERT_EQ(EventNumber::Triggered, callEvents.events(0).event_number());
    ASSERT_EQ(Platform::Web, callEvents.events(0).platform());
    ASSERT_FALSE(callEvents.events(0).has_id());
    ASSERT_FALSE(callEvents.events(0).has_user());
    ASSERT_FALSE(callEvents.events(0).has_inserttime());
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(PossiblePositions, MqttMobilusCoverControlServiceParamTest, Values(
    ExpectedPosition { "UP", 10000 },
    ExpectedPosition { "DOWN", 0 },
    ExpectedPosition { "23%", 2300 },
    ExpectedPosition { "50%", 5099 }
));
// clang-format on
