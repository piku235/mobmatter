#include "driving_adapters/mobilus/device_handlers/MobilusDeviceSyncerAdapter.h"
#include "application/model/MobilusDeviceType.h"
#include "common/logging/Logger.h"
#include "driving_adapters/mobilus/FakeDeviceSyncHandler.hpp"
#include "driving_adapters/mobilus/HandlerResult.h"
#include "driving_adapters/mobilus/MobilusDeviceEventHandler.h"
#include "driving_adapters/mobilus/MqttMobilusDeviceInfoSyncer.h"
#include "jungi/mobilus_gtw_client/EventNumber.h"
#include "jungi/mobilus_gtw_client/proto/CurrentStateResponse.pb.h"
#include "jungi/mobilus_gtw_client/proto/DevicesListResponse.pb.h"
#include "jungi/mobilus_gtw_client/proto/Event.pb.h"
#include "mobilus/MockMqttMobilusGtwClient.hpp"

#include <gtest/gtest.h>

using namespace mobmatter::driving_adapters::mobilus;
using namespace jungi::mobilus_gtw_client;
using mobmatter::application::model::MobilusDeviceType;
using mobmatter::common::logging::Logger;
using mobmatter::driving_adapters::mobilus::MobilusDeviceEventHandler;
using mobmatter::driving_adapters::mobilus::device_handlers::MobilusDeviceSyncerAdapter;
using mobmatter::tests::mobilus::MockMqttMobilusGtwClient;

namespace {

void setupResponses(proto::DevicesListResponse& deviceList, proto::CurrentStateResponse& currentState)
{
    auto sensoDev = deviceList.add_devices();
    sensoDev->set_id(2);
    sensoDev->set_name("senso");
    sensoDev->set_type(static_cast<int>(MobilusDeviceType::Senso));

    auto sensoState = currentState.add_events();
    sensoState->set_device_id(sensoDev->id());
    sensoState->set_value("100%");
    sensoState->set_event_number(EventNumber::Triggered);
}

}

TEST(MobilusDeviceSyncerAdapterTest, DoesNotRunSyncer)
{
    MockMqttMobilusGtwClient client;
    FakeDeviceSyncHandler handler(HandlerResult::Handled);
    MqttMobilusDeviceSyncer syncer(client, Logger::noop());
    MobilusDeviceSyncerAdapter adapter(syncer);

    proto::DevicesListResponse deviceList;
    proto::CurrentStateResponse currentState;

    setupResponses(deviceList, currentState);

    client.mockResponse(std::make_unique<proto::DevicesListResponse>(deviceList));
    client.mockResponse(std::make_unique<proto::CurrentStateResponse>(currentState));
    syncer.registerHandler(handler);

    proto::Event event;
    event.set_value("MODIFY");
    event.set_event_number(EventNumber::Device);

    ASSERT_EQ(HandlerResult::Unmatched, adapter.handle(event));
    ASSERT_TRUE(handler.seenDeviceEventPairs().empty());

    event.set_value("ADD");
    event.set_event_number(EventNumber::User);

    ASSERT_EQ(HandlerResult::Unmatched, adapter.handle(event));
    ASSERT_TRUE(handler.seenDeviceEventPairs().empty());
}

TEST(MobilusDeviceSyncerAdapterTest, RunsSyncer)
{
    MockMqttMobilusGtwClient client;
    FakeDeviceSyncHandler handler(HandlerResult::Handled);
    MqttMobilusDeviceSyncer syncer(client, Logger::noop());
    MobilusDeviceSyncerAdapter adapter(syncer);

    proto::DevicesListResponse deviceList;
    proto::CurrentStateResponse currentState;

    setupResponses(deviceList, currentState);

    client.mockResponse(std::make_unique<proto::DevicesListResponse>(deviceList));
    client.mockResponse(std::make_unique<proto::CurrentStateResponse>(currentState));
    syncer.registerHandler(handler);

    proto::Event event;
    event.set_value("ADD");
    event.set_event_number(EventNumber::Device);

    auto r = adapter.handle(event);

    ASSERT_EQ(HandlerResult::Handled, r);
    ASSERT_EQ(1, handler.seenDeviceEventPairs().size());
}
