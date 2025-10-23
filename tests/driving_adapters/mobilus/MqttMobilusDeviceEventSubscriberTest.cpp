#include "driving_adapters/mobilus/MqttMobilusDeviceEventSubscriber.h"
#include "FakeDeviceEventHandler.hpp"
#include "driving_adapters/mobilus/HandlerResult.h"
#include "jungi/mobilus_gtw_client/EventNumber.h"
#include "jungi/mobilus_gtw_client/proto/CallEvents.pb.h"
#include "mobilus/MockMqttMobilusGtwClient.hpp"

#include <gtest/gtest.h>

using namespace jungi::mobilus_gtw_client;
using namespace mobmatter::driving_adapters::mobilus;
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

TEST(MqttMobilusDeviceEventSubscriberTest, DoesNotDelegateEventsIfNoHandlers)
{
    MockMqttMobilusGtwClient client;
    MqttMobilusDeviceEventSubscriber eventSubscriber(client);

    eventSubscriber.boot();

    client.messageBus().dispatch(callEventsStub());
}

TEST(MqttMobilusDeviceEventSubscriberTest, DelegatesEventsToHandler)
{
    MockMqttMobilusGtwClient client;
    MqttMobilusDeviceEventSubscriber eventSubscriber(client);

    FakeDeviceEventHandler unmatchedHandler(HandlerResult::Unmatched);
    FakeDeviceEventHandler handledHandler(HandlerResult::Handled);
    FakeDeviceEventHandler otherHandler(HandlerResult::Unmatched);

    eventSubscriber.registerHandler(unmatchedHandler);
    eventSubscriber.registerHandler(handledHandler);
    eventSubscriber.registerHandler(otherHandler);
    eventSubscriber.boot();

    auto callEvents = callEventsStub();
    client.messageBus().dispatch(callEvents);

    ASSERT_EQ(2, unmatchedHandler.seenEvents().size());
    ASSERT_EQ(2, handledHandler.seenEvents().size());
    ASSERT_TRUE(otherHandler.seenEvents().empty());
}
