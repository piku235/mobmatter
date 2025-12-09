#include "driving_adapters/mobilus/device_handlers/MobilusDeviceSyncerAdapter.h"
#include "application/model/MobilusDeviceType.h"
#include "common/logging/Logger.h"
#include "driving_adapters/mobilus/FakeDeviceSyncHandler.hpp"
#include "driving_adapters/mobilus/MobilusDeviceEventHandler.h"
#include "driving_adapters/mobilus/MqttMobilusDeviceSyncer.h"
#include "jungi/mobgtw/EventNumber.h"
#include "jungi/mobgtw/proto/CurrentStateResponse.pb.h"
#include "jungi/mobgtw/proto/DevicesListResponse.pb.h"
#include "jungi/mobgtw/proto/Event.pb.h"
#include "mobilus/MockMqttMobilusGtwClient.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>

using namespace mobmatter::driving_adapters::mobilus;
using namespace jungi::mobgtw;
using mobmatter::application::model::MobilusDeviceType;
using mobmatter::common::logging::Logger;
using mobmatter::driving_adapters::mobilus::MobilusDeviceEventHandler;
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

class MobilusDeviceSyncerAdapterParamTest : public TestWithParam<std::string> { };

TEST(MobilusDeviceSyncerAdapterTest, DoesNotRunSync)
{
    MockMqttMobilusGtwClient client;
    FakeDeviceSyncHandler handler;
    MqttMobilusDeviceSyncer syncer(client, Logger::noop());
    MobilusDeviceSyncerAdapter adapter(syncer);

    syncer.registerHandler(handler);

    auto deviceList = std::make_unique<proto::DevicesListResponse>();
    setupClientResponse(*deviceList);

    client.mockResponse(std::move(deviceList));
    client.mockResponse(std::make_unique<proto::CurrentStateResponse>());

    proto::Event event;
    event.set_event_number(EventNumber::User);
    event.set_value("ADD");

    ASSERT_EQ(MobilusDeviceEventHandler::Result::Unmatched, adapter.handle(event));
    ASSERT_TRUE(handler.syncedDevices().empty());

    event.set_event_number(EventNumber::Device);
    event.set_value("UNKNOWN");

    ASSERT_EQ(MobilusDeviceEventHandler::Result::Unmatched, adapter.handle(event));
    ASSERT_TRUE(handler.syncedDevices().empty());
}

TEST_P(MobilusDeviceSyncerAdapterParamTest, RunsSync)
{
    MockMqttMobilusGtwClient client;
    FakeDeviceSyncHandler handler;
    MqttMobilusDeviceSyncer syncer(client, Logger::noop());
    MobilusDeviceSyncerAdapter adapter(syncer);

    auto deviceList = std::make_unique<proto::DevicesListResponse>();
    setupClientResponse(*deviceList);

    client.mockResponse(std::move(deviceList));
    client.mockResponse(std::make_unique<proto::CurrentStateResponse>());

    syncer.registerHandler(handler);

    proto::Event event;
    event.set_event_number(EventNumber::Device);
    event.set_value(GetParam());

    ASSERT_EQ(MobilusDeviceEventHandler::Result::Handled, adapter.handle(event));
    ASSERT_EQ(1, handler.syncedDevices().size());
}

INSTANTIATE_TEST_SUITE_P(DeviceEventValues, MobilusDeviceSyncerAdapterParamTest, Values("ADD", "MODIFY", "REMOVE"));
