#include "driving_adapters/mobilus/generic/MobilusDeviceNameSyncerAdapter.h"
#include "application/model/MobilusDeviceType.h"
#include "common/logging/Logger.h"
#include "driving_adapters/mobilus/FakeDeviceNameHandler.hpp"
#include "driving_adapters/mobilus/MobilusDeviceEventHandler.h"
#include "driving_adapters/mobilus/MqttMobilusDeviceNameSyncer.h"
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
using mobmatter::driving_adapters::mobilus::generic::MobilusDeviceNameSyncerAdapter;
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

TEST(MobilusDeviceNameSyncerAdapterTest, DoesNotRunSyncer)
{
    MockMqttMobilusGtwClient client;
    FakeDeviceNameHandler handler(MobilusDeviceType::Senso);
    MqttMobilusDeviceNameSyncer deviceNameSyncer(client, Logger::noop());
    MobilusDeviceNameSyncerAdapter adapter(deviceNameSyncer);

    client.mockResponse(std::make_unique<proto::DevicesListResponse>(deviceListResponseStub()));
    deviceNameSyncer.registerHandler(handler);

    proto::Event event;
    event.set_value("ADD");
    event.set_event_number(EventNumber::Device);

    ASSERT_EQ(MobilusDeviceEventHandler::Result::Unsupported, adapter.handle(event));
    ASSERT_TRUE(handler.handledNames.empty());

    event.set_value("MODIFY");
    event.set_event_number(EventNumber::User);

    ASSERT_EQ(MobilusDeviceEventHandler::Result::Unsupported, adapter.handle(event));
    ASSERT_TRUE(handler.handledNames.empty());
}

TEST(MobilusDeviceNameSyncerAdapterTest, RunsSyncer)
{
    MockMqttMobilusGtwClient client;
    FakeDeviceNameHandler handler(MobilusDeviceType::Senso);
    MqttMobilusDeviceNameSyncer syncer(client, Logger::noop());
    MobilusDeviceNameSyncerAdapter adapter(syncer);

    client.mockResponse(std::make_unique<proto::DevicesListResponse>(deviceListResponseStub()));
    syncer.registerHandler(handler);

    proto::Event event;
    event.set_value("MODIFY");
    event.set_event_number(EventNumber::Device);

    auto r = adapter.handle(event);

    ASSERT_EQ(MobilusDeviceEventHandler::Result::Handled, r);
    ASSERT_EQ(1, handler.handledNames.size());
}
