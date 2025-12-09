#include "driving_adapters/mobilus/MqttMobilusDeviceSyncer.h"
#include "FakeDeviceSyncHandler.hpp"
#include "application/model/MobilusDeviceType.h"
#include "common/logging/Logger.h"
#include "driving_adapters/mobilus/MobilusDeviceSyncHandler.h"
#include "jungi/mobgtw/EventNumber.h"
#include "jungi/mobgtw/proto/CurrentStateResponse.pb.h"
#include "jungi/mobgtw/proto/DevicesListResponse.pb.h"
#include "jungi/mobgtw/proto/Event.pb.h"
#include "mobilus/MockMqttMobilusGtwClient.hpp"

#include <gtest/gtest.h>

using namespace jungi::mobgtw;
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

    auto sensoDev = deviceList.add_devices();
    sensoDev->set_id(2);
    sensoDev->set_name("senso");
    sensoDev->set_type(static_cast<int>(MobilusDeviceType::Senso));

    // missing current state
    auto cmrDev = deviceList.add_devices();
    cmrDev->set_id(3);
    cmrDev->set_name("cmr");
    cmrDev->set_type(static_cast<int>(MobilusDeviceType::Cmr));

    auto sensoState = currentState.add_events();
    sensoState->set_device_id(sensoDev->id());
    sensoState->set_value("100%");
    sensoState->set_event_number(EventNumber::Triggered);

    auto switchState = currentState.add_events();
    switchState->set_device_id(switchDev->id());
    switchState->set_value("ON");
    switchState->set_event_number(EventNumber::Reached);
}

}

TEST(MqttMobilusDeviceSyncerTest, DoesNotDelegateDeviceEventPairIfNoHandlers)
{
    MockMqttMobilusGtwClient client;
    MqttMobilusDeviceSyncer syncer(client, Logger::noop());

    client.mockResponse(std::make_unique<proto::DevicesListResponse>());
    client.mockResponse(std::make_unique<proto::CurrentStateResponse>());

    syncer.run();
}

TEST(MqttMobilusDeviceSyncerTest, DelegatesDevicesToHandlers)
{
    MockMqttMobilusGtwClient client;
    MqttMobilusDeviceSyncer syncer(client, Logger::noop());

    FakeDeviceSyncHandler handler;
    FakeDeviceSyncHandler otherHandler;

    syncer.registerHandler(handler);
    syncer.registerHandler(otherHandler);

    proto::DevicesListResponse deviceList;
    proto::CurrentStateResponse currentState;

    setupResponses(deviceList, currentState);

    syncer.run();
    ASSERT_TRUE(handler.syncedDevices().empty());
    ASSERT_TRUE(otherHandler.syncedDevices().empty());

    client.mockResponse(std::make_unique<proto::DevicesListResponse>(deviceList));
    syncer.run();

    ASSERT_TRUE(handler.syncedDevices().empty());
    ASSERT_TRUE(otherHandler.syncedDevices().empty());

    client.mockResponse(std::make_unique<proto::DevicesListResponse>(deviceList));
    client.mockResponse(std::make_unique<proto::CurrentStateResponse>(currentState));
    syncer.run();

    ASSERT_EQ(3, handler.syncedDevices().size());
    ASSERT_EQ(3, otherHandler.syncedDevices().size());

    const auto& switchDev = handler.syncedDevices().at(deviceList.devices(0).id());

    ASSERT_EQ(deviceList.devices(0).SerializeAsString(), switchDev.device.SerializeAsString());
    ASSERT_EQ(currentState.events(1).SerializeAsString(), switchDev.lastEvent.SerializeAsString());

    const auto& sensoDev = handler.syncedDevices().at(deviceList.devices(1).id());

    ASSERT_EQ(deviceList.devices(1).SerializeAsString(), sensoDev.device.SerializeAsString());
    ASSERT_EQ(currentState.events(0).SerializeAsString(), sensoDev.lastEvent.SerializeAsString());

    const auto& cmrDev = handler.syncedDevices().at(deviceList.devices(2).id());

    ASSERT_EQ(deviceList.devices(2).SerializeAsString(), cmrDev.device.SerializeAsString());
    ASSERT_EQ((proto::Event()).SerializeAsString(), cmrDev.lastEvent.SerializeAsString());
}
