#include "driving_adapters/mobilus/device_handlers/MobilusDeviceInfoSyncerAdapter.h"
#include "application/model/MobilusDeviceType.h"
#include "common/logging/Logger.h"
#include "driving_adapters/mobilus/FakeDeviceInfoHandler.hpp"
#include "driving_adapters/mobilus/HandlerResult.h"
#include "driving_adapters/mobilus/MobilusDeviceEventHandler.h"
#include "driving_adapters/mobilus/MqttMobilusDeviceInfoSyncer.h"
#include "jungi/mobilus_gtw_client/EventNumber.h"
#include "jungi/mobilus_gtw_client/proto/DevicesListResponse.pb.h"
#include "jungi/mobilus_gtw_client/proto/Event.pb.h"
#include "mobilus/MockMqttMobilusGtwClient.hpp"

#include <gtest/gtest.h>

using namespace mobmatter::driving_adapters::mobilus;
using namespace jungi::mobilus_gtw_client;
using mobmatter::application::model::MobilusDeviceType;
using mobmatter::common::logging::Logger;
using mobmatter::driving_adapters::mobilus::MobilusDeviceEventHandler;
using mobmatter::driving_adapters::mobilus::device_handlers::MobilusDeviceInfoSyncerAdapter;
using mobmatter::tests::mobilus::MockMqttMobilusGtwClient;

namespace {

auto deviceListResponseStub()
{
    proto::DevicesListResponse response;

    auto devSenso = response.add_devices();
    devSenso->set_id(1);
    devSenso->set_name("Senso");
    devSenso->set_type(static_cast<google::protobuf::int32>(MobilusDeviceType::Senso));

    return response;
}

}

TEST(MobilusDeviceInfoSyncerAdapterTest, DoesNotRunSyncer)
{
    MockMqttMobilusGtwClient client;
    FakeDeviceInfoHandler handler(HandlerResult::Handled);
    MqttMobilusDeviceInfoSyncer syncer(client, Logger::noop());
    MobilusDeviceInfoSyncerAdapter adapter(syncer);

    client.mockResponse(std::make_unique<proto::DevicesListResponse>(deviceListResponseStub()));
    syncer.registerHandler(handler);

    proto::Event event;
    event.set_value("ADD");
    event.set_event_number(EventNumber::Device);

    ASSERT_EQ(HandlerResult::Unmatched, adapter.handle(event));
    ASSERT_TRUE(handler.seenDevices().empty());

    event.set_value("MODIFY");
    event.set_event_number(EventNumber::User);

    ASSERT_EQ(HandlerResult::Unmatched, adapter.handle(event));
    ASSERT_TRUE(handler.seenDevices().empty());
}

TEST(MobilusDeviceInfoSyncerAdapterTest, RunsSyncer)
{
    MockMqttMobilusGtwClient client;
    FakeDeviceInfoHandler handler(HandlerResult::Handled);
    MqttMobilusDeviceInfoSyncer syncer(client, Logger::noop());
    MobilusDeviceInfoSyncerAdapter adapter(syncer);

    client.mockResponse(std::make_unique<proto::DevicesListResponse>(deviceListResponseStub()));
    syncer.registerHandler(handler);

    proto::Event event;
    event.set_value("MODIFY");
    event.set_event_number(EventNumber::Device);

    auto r = adapter.handle(event);

    ASSERT_EQ(HandlerResult::Handled, r);
    ASSERT_EQ(1, handler.seenDevices().size());
}
