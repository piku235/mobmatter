#include "application/model/window_covering/Position.h"
#include "common/logging/Logger.h"
#include "driven_adapters/mobilus/MqttMobilusDeviceControlService.h"
#include "mobilus/MockMqttMobilusGtwClient.hpp"

#include <gtest/gtest.h>
#include <jungi/mobgtw/EventNumber.h>
#include <jungi/mobgtw/MessageType.h>
#include <jungi/mobgtw/Platform.h>
#include <jungi/mobgtw/ProtoUtils.h>
#include <jungi/mobgtw/proto/CallEvents.pb.h>

using namespace jungi::mobgtw;
using mobmatter::application::model::MobilusDeviceId;
using mobmatter::application::model::Percent;
using mobmatter::application::model::window_covering::Position;
using mobmatter::common::logging::Logger;
using mobmatter::driven_adapters::mobilus::MqttMobilusDeviceControlService;
using mobmatter::tests::mobilus::MockMqttMobilusGtwClient;

static constexpr MobilusDeviceId kDeviceId = 12;

namespace {

void assertSentCallEvent(const MockMqttMobilusGtwClient& client, const std::string& expectedEventValue)
{
    ASSERT_EQ(1, client.sentMessages().size());
    ASSERT_EQ(MessageType::CallEvents, ProtoUtils::messageTypeFor(*client.sentMessages()[0]));

    auto& callEvents = static_cast<const proto::CallEvents&>(*client.sentMessages()[0]);

    ASSERT_EQ(1, callEvents.events_size());
    ASSERT_EQ(kDeviceId, callEvents.events(0).device_id());
    ASSERT_EQ(expectedEventValue, callEvents.events(0).value());
    ASSERT_EQ(EventNumber::Triggered, callEvents.events(0).event_number());
    ASSERT_EQ(Platform::Web, callEvents.events(0).platform());
    ASSERT_FALSE(callEvents.events(0).has_id());
    ASSERT_FALSE(callEvents.events(0).has_user());
    ASSERT_FALSE(callEvents.events(0).has_inserttime());
}

}

TEST(MqttMobilusCoverControlServiceTest, OpensCover)
{
    MockMqttMobilusGtwClient client;
    MqttMobilusDeviceControlService coverControlService(client, Logger::noop());

    coverControlService.openCover(kDeviceId);

    assertSentCallEvent(client, "UP");
}

TEST(MqttMobilusCoverControlServiceTest, ClosesCover)
{
    MockMqttMobilusGtwClient client;
    MqttMobilusDeviceControlService coverControlService(client, Logger::noop());

    coverControlService.closeCover(kDeviceId);

    assertSentCallEvent(client, "DOWN");
}

TEST(MqttMobilusCoverControlServiceTest, LiftsCover)
{
    MockMqttMobilusGtwClient client;
    MqttMobilusDeviceControlService coverControlService(client, Logger::noop());

    coverControlService.liftCover(kDeviceId, Position::fullyOpen());

    assertSentCallEvent(client, "100%");
}

TEST(MqttMobilusCoverControlServiceTest, TiltsCover)
{
    MockMqttMobilusGtwClient client;
    MqttMobilusDeviceControlService coverControlService(client, Logger::noop());

    coverControlService.tiltCover(kDeviceId, Position::fullyOpen());

    assertSentCallEvent(client, "100$");
}

TEST(MqttMobilusCoverControlServiceTest, StopsCoverMotion)
{
    MockMqttMobilusGtwClient client;
    MqttMobilusDeviceControlService coverControlService(client, Logger::noop());

    coverControlService.stopCoverMotion(kDeviceId);

    assertSentCallEvent(client, "STOP");
}
