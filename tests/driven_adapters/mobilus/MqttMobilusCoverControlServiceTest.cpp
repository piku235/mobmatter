#include "driven_adapters/mobilus/MqttMobilusCoverControlService.h"
#include "application/model/window_covering/Position.h"
#include "jungi/mobilus_gtw_client/EventNumber.h"
#include "jungi/mobilus_gtw_client/MessageType.h"
#include "jungi/mobilus_gtw_client/Platform.h"
#include "jungi/mobilus_gtw_client/ProtoUtils.h"
#include "jungi/mobilus_gtw_client/proto/CallEvents.pb.h"
#include "mobilus/FakeMqttMobilusGtwClient.h"

#include <gtest/gtest.h>

#include <tuple>

using namespace jungi::mobilus_gtw_client;
using mmbridge::application::model::Percent;
using mmbridge::application::model::window_covering::Position;
using mmbridge::common::logging::Logger;
using mmbridge::driven_adapters::mobilus::MqttMobilusCoverControlService;
using mmbridge::tests::mobilus::FakeMqttMobilusGtwClient;
using testing::TestWithParam;
using testing::Values;
using ExpectedPosition = std::tuple<std::string, Position>;

class MqttMobilusCoverControlServiceTest : public TestWithParam<ExpectedPosition> { };

TEST_P(MqttMobilusCoverControlServiceTest, LiftsCover)
{
    FakeMqttMobilusGtwClient client;
    MqttMobilusCoverControlService coverControlService(client, Logger::noop());

    coverControlService.liftCover(23, Position::fullyOpen());

    EXPECT_EQ(1, client.sentMessages.size());
    EXPECT_EQ(MessageType::CallEvents, ProtoUtils::messageTypeFor(*client.sentMessages[0]));

    auto& callEvents = static_cast<const proto::CallEvents&>(*client.sentMessages[0]);

    EXPECT_EQ(1, callEvents.events_size());
    EXPECT_EQ(23, callEvents.events(0).device_id());
    EXPECT_EQ("UP", callEvents.events(0).value());
    EXPECT_EQ(EventNumber::Triggered, callEvents.events(0).event_number());
    EXPECT_EQ(Platform::Web, callEvents.events(0).platform());
    EXPECT_FALSE(callEvents.events(0).has_id());
    EXPECT_FALSE(callEvents.events(0).has_user());
    EXPECT_FALSE(callEvents.events(0).has_inserttime());
}

TEST(MqttMobilusCoverControlServiceTest, StopsCoverMotion)
{
    FakeMqttMobilusGtwClient client;
    MqttMobilusCoverControlService coverControlService(client, Logger::noop());

    coverControlService.stopCoverMotion(23);

    EXPECT_EQ(1, client.sentMessages.size());
    EXPECT_EQ(MessageType::CallEvents, ProtoUtils::messageTypeFor(*client.sentMessages[0]));

    auto& callEvents = static_cast<const proto::CallEvents&>(*client.sentMessages[0]);

    EXPECT_EQ(1, callEvents.events_size());
    EXPECT_EQ(23, callEvents.events(0).device_id());
    EXPECT_EQ("STOP", callEvents.events(0).value());
    EXPECT_EQ(EventNumber::Triggered, callEvents.events(0).event_number());
    EXPECT_EQ(Platform::Web, callEvents.events(0).platform());
    EXPECT_FALSE(callEvents.events(0).has_id());
    EXPECT_FALSE(callEvents.events(0).has_user());
    EXPECT_FALSE(callEvents.events(0).has_inserttime());
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(PossiblePositions, MqttMobilusCoverControlServiceTest, Values(
    ExpectedPosition { "UP", Position::fullyOpen() },
    ExpectedPosition { "DOWN", Position::fullyClosed() },
    ExpectedPosition { "23%", Position::open(*Percent::from(23)) },
    ExpectedPosition { "50%", Position::open(*Percent::from100ths(5099)) }
));
// clang-format on
