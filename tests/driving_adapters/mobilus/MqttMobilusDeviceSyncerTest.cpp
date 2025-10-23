#include "driving_adapters/mobilus/MqttMobilusDeviceSyncer.h"
#include "FakeDeviceSyncHandler.hpp"
#include "application/model/MobilusDeviceType.h"
#include "driving_adapters/mobilus/MobilusDeviceSyncHandler.h"
#include "jungi/mobilus_gtw_client/EventNumber.h"
#include "jungi/mobilus_gtw_client/proto/CurrentStateResponse.pb.h"
#include "jungi/mobilus_gtw_client/proto/DevicesListResponse.pb.h"
#include "mobilus/MockMqttMobilusGtwClient.hpp"

#include <gtest/gtest.h>

using namespace jungi::mobilus_gtw_client;
using namespace mobmatter::driving_adapters::mobilus;
using mobmatter::application::model::MobilusDeviceType;
using mobmatter::common::logging::Logger;
using mobmatter::tests::mobilus::MockMqttMobilusGtwClient;

namespace {

void setupResponses(proto::DevicesListResponse& deviceList, proto::CurrentStateResponse& currentState)
{
    auto switchDev = deviceList.add_devices();
    switchDev->set_id(1);
    switchDev->set_name("switch");
    switchDev->set_type(static_cast<int>(MobilusDeviceType::Switch));

    auto switchState = currentState.add_events();
    switchState->set_device_id(switchDev->id());
    switchState->set_value("ON");
    switchState->set_event_number(EventNumber::Reached);

    auto sensoDev = deviceList.add_devices();
    sensoDev->set_id(2);
    sensoDev->set_name("senso");
    sensoDev->set_type(static_cast<int>(MobilusDeviceType::Senso));

    auto sensoState = currentState.add_events();
    sensoState->set_device_id(sensoDev->id());
    sensoState->set_value("100%");
    sensoState->set_event_number(EventNumber::Triggered);

    // missing current state
    auto cmrDev = deviceList.add_devices();
    cmrDev->set_id(3);
    cmrDev->set_name("cmr");
    cmrDev->set_type(static_cast<int>(MobilusDeviceType::Cmr));
}

}

TEST(MqttMobilusDeviceSyncerTest, DoesNotDelegateDeviceEventPairIfNoHandlers)
{
    MockMqttMobilusGtwClient client;
    MqttMobilusDeviceSyncer syncer(client, Logger::noop());

    client.mockResponse(std::make_unique<proto::DevicesListResponse>());
    client.mockResponse(std::make_unique<proto::CurrentStateResponse>());

    syncer.boot();
}

TEST(MqttMobilusDeviceSyncerTest, DelegatesDeviceEventPairToHandler)
{
    MockMqttMobilusGtwClient client;
    MqttMobilusDeviceSyncer syncer(client, Logger::noop());

    FakeDeviceSyncHandler unmatchedHandler(HandlerResult::Unmatched);
    FakeDeviceSyncHandler handledHandler(HandlerResult::Handled);
    FakeDeviceSyncHandler otherHandler(HandlerResult::Unmatched);

    syncer.registerHandler(unmatchedHandler);
    syncer.registerHandler(handledHandler);
    syncer.registerHandler(otherHandler);

    proto::DevicesListResponse deviceList;
    proto::CurrentStateResponse currentState;

    setupResponses(deviceList, currentState);

    syncer.boot();
    ASSERT_TRUE(unmatchedHandler.seenDeviceEventPairs().empty());
    ASSERT_TRUE(handledHandler.seenDeviceEventPairs().empty());
    ASSERT_TRUE(otherHandler.seenDeviceEventPairs().empty());

    client.mockResponse(std::make_unique<proto::DevicesListResponse>(deviceList));
    syncer.boot();

    ASSERT_TRUE(unmatchedHandler.seenDeviceEventPairs().empty());
    ASSERT_TRUE(handledHandler.seenDeviceEventPairs().empty());
    ASSERT_TRUE(otherHandler.seenDeviceEventPairs().empty());

    client.mockResponse(std::make_unique<proto::DevicesListResponse>(deviceList));
    client.mockResponse(std::make_unique<proto::CurrentStateResponse>(currentState));
    syncer.boot();

    ASSERT_EQ(2, unmatchedHandler.seenDeviceEventPairs().size());
    ASSERT_EQ(2, handledHandler.seenDeviceEventPairs().size());
    ASSERT_TRUE(otherHandler.seenDeviceEventPairs().empty());
}
