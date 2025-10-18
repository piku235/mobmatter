#include "driving_adapters/mobilus/MqttMobilusDeviceStateSyncer.h"
#include "FakeDeviceEventHandler.hpp"
#include "common/logging/Logger.h"
#include "driving_adapters/mobilus/MobilusDeviceEventHandler.h"
#include "jungi/mobilus_gtw_client/EventNumber.h"
#include "jungi/mobilus_gtw_client/proto/CurrentStateResponse.pb.h"
#include "mobilus/MockMqttMobilusGtwClient.hpp"

#include <gtest/gtest.h>

#include <vector>

using namespace jungi::mobilus_gtw_client;
using mobmatter::common::logging::Logger;
using mobmatter::driving_adapters::mobilus::MobilusDeviceEventHandler;
using mobmatter::driving_adapters::mobilus::MqttMobilusDeviceStateSyncer;
using mobmatter::tests::mobilus::MockMqttMobilusGtwClient;

namespace {

auto currentStateResponseStub()
{
    proto::CurrentStateResponse response;

    auto switchEvent = response.add_events();
    switchEvent->set_device_id(1);
    switchEvent->set_value("OFF");
    switchEvent->set_event_number(EventNumber::Reached);

    auto coverEvent = response.add_events();
    coverEvent->set_device_id(2);
    coverEvent->set_value("0%");
    coverEvent->set_event_number(EventNumber::Sent);

    return response;
}

}

TEST(MqttMobilusDeviceStateSyncerTest, Runs)
{
    std::vector<proto::Event> handledEvents;
    FakeDeviceEventHandler eventHandler(handledEvents);
    MockMqttMobilusGtwClient client;
    MqttMobilusDeviceStateSyncer syncer(client, eventHandler, Logger::noop());

    auto currentState = currentStateResponseStub();
    client.mockResponse(std::make_unique<proto::CurrentStateResponse>(currentState));

    syncer.run();

    ASSERT_EQ(2, handledEvents.size());
    for (int i = 0; i < 2; i++) {
        ASSERT_EQ(currentState.events(i).device_id(), handledEvents[i].device_id());
        ASSERT_EQ(currentState.events(i).value(), handledEvents[i].value());
        ASSERT_EQ(currentState.events(i).event_number(), handledEvents[i].event_number());
    }
}
