#include "driving_adapters/mobilus/MqttMobilusDeviceStateSyncer.h"
#include "FakeEventHandler.hpp"
#include "common/logging/Logger.h"
#include "driving_adapters/mobilus/MobilusEventHandler.h"
#include "jungi/mobilus_gtw_client/EventNumber.h"
#include "jungi/mobilus_gtw_client/proto/CurrentStateResponse.pb.h"
#include "mobilus/MockMqttMobilusGtwClient.hpp"

#include <gtest/gtest.h>

#include <memory>
#include <vector>

using namespace jungi::mobilus_gtw_client;
using mmbridge::common::logging::Logger;
using mmbridge::driving_adapters::mobilus::MobilusEventHandler;
using mmbridge::driving_adapters::mobilus::MqttMobilusDeviceStateSyncer;
using mmbridge::tests::mobilus::MockMqttMobilusGtwClient;

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
    FakeEventHandler eventHandler(handledEvents);
    MockMqttMobilusGtwClient client;
    MqttMobilusDeviceStateSyncer eventSubscriber(client, eventHandler, Logger::noop());

    auto currentState = currentStateResponseStub();
    client.mockResponse(std::make_unique<proto::CurrentStateResponse>(currentState));

    eventSubscriber.run();

    ASSERT_EQ(2, handledEvents.size());
    for (int i = 0; i < 2; i++) {
        ASSERT_EQ(currentState.events(i).device_id(), handledEvents[i].device_id());
        ASSERT_EQ(currentState.events(i).value(), handledEvents[i].value());
        ASSERT_EQ(currentState.events(i).event_number(), handledEvents[i].event_number());
    }
}
