#include "driving_adapters/mobilus/device_handlers/MobilusDeviceSyncerAdapter.h"
#include "application/model/MobilusDeviceType.h"
#include "common/logging/Logger.h"
#include "driving_adapters/mobilus/FakeDeviceSyncHandler.hpp"
#include "driving_adapters/mobilus/MqttMobilusDeviceSyncer.h"
#include "jungi/mobilus_gtw_client/EventNumber.h"
#include "jungi/mobilus_gtw_client/proto/CurrentStateResponse.pb.h"
#include "jungi/mobilus_gtw_client/proto/DevicesListResponse.pb.h"
#include "jungi/mobilus_gtw_client/proto/Event.pb.h"
#include "mobilus/MockMqttMobilusGtwClient.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>

using namespace mobmatter::driving_adapters::mobilus;
using namespace jungi::mobilus_gtw_client;
using mobmatter::application::model::MobilusDeviceType;
using mobmatter::common::logging::Logger;
using mobmatter::driving_adapters::mobilus::device_handlers::MobilusDeviceSyncerAdapter;
using mobmatter::tests::mobilus::MockMqttMobilusGtwClient;
using testing::TestWithParam;
using testing::Values;

namespace {

void setupClientResponse(proto::DevicesListResponse& deviceList)
{
    auto switchDev = deviceList.add_devices();
    switchDev->set_id(1);
    switchDev->set_name("switch");
    switchDev->set_type(static_cast<int>(MobilusDeviceType::Switch));
}

}

class MobilusDeviceSyncerAdapterTest : public TestWithParam<std::string> {
};

TEST(MobilusDeviceSyncerAdapterTest, DoesNotRunSync)
{
    MockMqttMobilusGtwClient client;
    MqttMobilusDeviceSyncer syncer(client, Logger::noop());
    MobilusDeviceSyncerAdapter adapter(syncer);
    FakeDeviceSyncHandler handler;

    auto deviceList = std::make_unique<proto::DevicesListResponse>();
    setupClientResponse(*deviceList);

    client.mockResponse(std::move(deviceList));
    client.mockResponse(std::make_unique<proto::CurrentStateResponse>());

    syncer.registerHandler(handler);

    proto::Event event;
    event.set_event_number(EventNumber::User);
    event.set_value("ADD");

    adapter.handle(event);
    ASSERT_TRUE(handler.syncedDevices().empty());

    event.set_event_number(EventNumber::Device);
    event.set_value("UNKNOWN");

    adapter.handle(event);
    ASSERT_TRUE(handler.syncedDevices().empty());
}

TEST_P(MobilusDeviceSyncerAdapterTest, RunsSync)
{
    MockMqttMobilusGtwClient client;
    MqttMobilusDeviceSyncer syncer(client, Logger::noop());
    MobilusDeviceSyncerAdapter adapter(syncer);
    FakeDeviceSyncHandler handler;

    auto deviceList = std::make_unique<proto::DevicesListResponse>();
    setupClientResponse(*deviceList);

    client.mockResponse(std::move(deviceList));
    client.mockResponse(std::make_unique<proto::CurrentStateResponse>());

    syncer.registerHandler(handler);

    proto::Event event;
    event.set_event_number(EventNumber::Device);
    event.set_value(GetParam());

    adapter.handle(event);
    ASSERT_EQ(1, handler.syncedDevices().size());
}

INSTANTIATE_TEST_SUITE_P(EventValues, MobilusDeviceSyncerAdapterTest, Values("ADD", "MODIFY", "REMOVE"));
