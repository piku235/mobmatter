#include "FakeDeviceEventHandler.hpp"
#include "driving_adapters/mobilus/MobilusDeviceEventHandler.h"
#include "driving_adapters/mobilus/MqttMobilusDeviceEventSubscriber.h"
#include "jungi/mobilus_gtw_client/EventNumber.h"
#include "jungi/mobilus_gtw_client/proto/CallEvents.pb.h"
#include "mobilus/MockMqttMobilusGtwClient.hpp"

#include <gtest/gtest.h>

#include <vector>

using namespace jungi::mobilus_gtw_client;
using mobmatter::driving_adapters::mobilus::MobilusDeviceEventHandler;
using mobmatter::driving_adapters::mobilus::MqttMobilusDeviceEventSubscriber;
using mobmatter::tests::mobilus::MockMqttMobilusGtwClient;

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

TEST(MqttMobilusDeviceEventSubscriberTest, DelegatesEventsToEventHandler)
{
    std::vector<proto::Event> handledEvents;
    FakeDeviceEventHandler eventHandler(handledEvents);
    MockMqttMobilusGtwClient client;
    MqttMobilusDeviceEventSubscriber eventSubscriber(client, eventHandler);
    eventSubscriber.boot();

    auto callEvents = callEventsStub();
    client.messageBus().dispatch(callEvents);

    ASSERT_EQ(2, handledEvents.size());
    for (int i = 0; i < 2; i++) {
        ASSERT_EQ(callEvents.events(i).device_id(), handledEvents[i].device_id());
        ASSERT_EQ(callEvents.events(i).value(), handledEvents[i].value());
        ASSERT_EQ(callEvents.events(i).event_number(), handledEvents[i].event_number());
    }
}
