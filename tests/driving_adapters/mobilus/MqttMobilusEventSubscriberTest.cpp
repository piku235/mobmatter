#include "driving_adapters/mobilus/MqttMobilusEventSubscriber.h"
#include "FakeEventHandler.hpp"
#include "driving_adapters/mobilus/MobilusEventHandler.h"
#include "jungi/mobilus_gtw_client/EventNumber.h"
#include "jungi/mobilus_gtw_client/proto/CallEvents.pb.h"
#include "mobilus/MockMqttMobilusGtwClient.hpp"

#include <gtest/gtest.h>

#include <memory>
#include <vector>

using namespace jungi::mobilus_gtw_client;
using mmbridge::driving_adapters::mobilus::MobilusEventHandler;
using mmbridge::driving_adapters::mobilus::MqttMobilusEventSubscriber;
using mmbridge::tests::mobilus::MockMqttMobilusGtwClient;

namespace {

auto callEventsStub()
{
    proto::CallEvents callEvents;

    auto switchEvent = callEvents.add_events();
    switchEvent->set_device_id(1);
    switchEvent->set_value("OFF");
    switchEvent->set_event_number(EventNumber::Reached);

    auto coverEvent = callEvents.add_events();
    coverEvent->set_device_id(2);
    coverEvent->set_value("0%");
    coverEvent->set_event_number(EventNumber::Sent);

    return callEvents;
}

}

TEST(MqttMobilusEventSubscriberTest, DelegatesEventsToEventHandler)
{
    std::vector<proto::Event> handledEvents;
    FakeEventHandler eventHandler(handledEvents);
    MockMqttMobilusGtwClient client;
    MqttMobilusEventSubscriber eventSubscriber(client, eventHandler);
    eventSubscriber.boot();

    auto callEvents = callEventsStub();
    client.messageBus().dispatch(callEvents);

    EXPECT_EQ(2, handledEvents.size());
    for (int i = 0; i < 2; i++) {
        EXPECT_EQ(callEvents.events(i).device_id(), handledEvents[i].device_id());
        EXPECT_EQ(callEvents.events(i).value(), handledEvents[i].value());
        EXPECT_EQ(callEvents.events(i).event_number(), handledEvents[i].event_number());
    }
}
