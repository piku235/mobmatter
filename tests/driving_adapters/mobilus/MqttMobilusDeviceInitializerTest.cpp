#include "driving_adapters/mobilus/MqttMobilusDeviceInitializer.h"
#include "application/model/MobilusDeviceType.h"
#include "driving_adapters/mobilus/MobilusDeviceInitHandler.h"
#include "jungi/mobilus_gtw_client/EventNumber.h"
#include "jungi/mobilus_gtw_client/proto/CurrentStateResponse.pb.h"
#include "jungi/mobilus_gtw_client/proto/DevicesListResponse.pb.h"
#include "mobilus/MockMqttMobilusGtwClient.hpp"

#include <gtest/gtest.h>

#include <vector>

using namespace jungi::mobilus_gtw_client;
using mobmatter::application::model::MobilusDeviceType;
using mobmatter::common::logging::Logger;
using mobmatter::driving_adapters::mobilus::MobilusDeviceInitHandler;
using mobmatter::driving_adapters::mobilus::MqttMobilusDeviceInitializer;
using mobmatter::tests::mobilus::MockMqttMobilusGtwClient;

namespace {

class FakeDeviceInitHandler final : public MobilusDeviceInitHandler {
public:
    struct InitiatedDevice {
        proto::Device device;
        proto::Event currentState;
    };

    FakeDeviceInitHandler(std::vector<InitiatedDevice>& initiatedDevices)
        : mInitiatedDevices(initiatedDevices)
    {
    }

    void handle(const proto::Device& device, const proto::Event& currentState) override
    {
        mInitiatedDevices.push_back({ device, currentState });
    }

    bool supports(MobilusDeviceType deviceType) const override
    {
        return MobilusDeviceType::Senso == deviceType
            || MobilusDeviceType::Cmr == deviceType;
    }

private:
    std::vector<InitiatedDevice>& mInitiatedDevices;
};

void setupResponses(proto::DevicesListResponse& deviceList, proto::CurrentStateResponse& currentState)
{
    // unsupported
    auto switchDev = deviceList.add_devices();
    switchDev->set_id(1);
    switchDev->set_name("switch");
    switchDev->set_type(static_cast<int>(MobilusDeviceType::Switch));

    auto switchState = currentState.add_events();
    switchState->set_device_id(switchState->id());
    switchState->set_value("ON");
    switchState->set_event_number(EventNumber::Reached);

    // supported
    auto sensoDev = deviceList.add_devices();
    sensoDev->set_id(2);
    sensoDev->set_name("senso");
    sensoDev->set_type(static_cast<int>(MobilusDeviceType::Senso));

    auto sensoState = currentState.add_events();
    sensoState->set_device_id(sensoDev->id());
    sensoState->set_value("100%");
    sensoState->set_event_number(EventNumber::Triggered);

    // event number: sent
    auto cmrDev = deviceList.add_devices();
    cmrDev->set_id(3);
    cmrDev->set_name("cmr");
    cmrDev->set_type(static_cast<int>(MobilusDeviceType::Cmr));

    auto cmrState = currentState.add_events();
    cmrState->set_device_id(cmrDev->id());
    cmrState->set_value("0%");
    cmrState->set_event_number(EventNumber::Sent);
}

}

TEST(MqttMobilusDeviceInitializerTest, Initializes)
{
    MockMqttMobilusGtwClient client;
    MqttMobilusDeviceInitializer initializer(client, Logger::noop());

    std::vector<FakeDeviceInitHandler::InitiatedDevice> initiatedDevices;
    FakeDeviceInitHandler handler(initiatedDevices);
    initializer.registerHandler(handler);

    proto::DevicesListResponse deviceList;
    proto::CurrentStateResponse currentState;

    setupResponses(deviceList, currentState);

    initializer.run();
    ASSERT_EQ(0, initiatedDevices.size());

    client.mockResponse(std::make_unique<proto::DevicesListResponse>(deviceList));
    initializer.run();
    ASSERT_EQ(0, initiatedDevices.size());

    client.mockResponse(std::make_unique<proto::DevicesListResponse>(deviceList));
    client.mockResponse(std::make_unique<proto::CurrentStateResponse>(currentState));
    initializer.run();
    ASSERT_EQ(2, initiatedDevices.size());

    for (auto i : { 1, 2 }) {
        ASSERT_EQ(deviceList.devices(i).id(), initiatedDevices[i - 1].device.id());
        ASSERT_EQ(deviceList.devices(i).name(), initiatedDevices[i - 1].device.name());
        ASSERT_EQ(deviceList.devices(i).type(), initiatedDevices[i - 1].device.type());
        ASSERT_EQ(currentState.events(i).device_id(), initiatedDevices[i - 1].currentState.device_id());
        ASSERT_EQ(currentState.events(i).value(), initiatedDevices[i - 1].currentState.value());
        ASSERT_EQ(currentState.events(i).event_number(), initiatedDevices[i - 1].currentState.event_number());
    }
}

TEST(MqttMobilusDeviceInitializerTest, InitializesNone)
{
    MockMqttMobilusGtwClient client;
    MqttMobilusDeviceInitializer initializer(client, Logger::noop());

    std::vector<FakeDeviceInitHandler::InitiatedDevice> initiatedDevices;
    FakeDeviceInitHandler handler(initiatedDevices);
    initializer.registerHandler(handler);

    client.mockResponse(std::make_unique<proto::DevicesListResponse>());
    client.mockResponse(std::make_unique<proto::CurrentStateResponse>());

    initializer.run();

    ASSERT_EQ(0, initiatedDevices.size());
}
