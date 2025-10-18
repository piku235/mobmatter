#include "driving_adapters/mobilus/MqttMobilusDeviceNameSyncer.h"
#include "application/model/MobilusDeviceType.h"
#include "common/logging/Logger.h"
#include "driving_adapters/mobilus/MobilusDeviceNameHandler.h"
#include "jungi/mobilus_gtw_client/EventNumber.h"
#include "jungi/mobilus_gtw_client/proto/DevicesListResponse.pb.h"
#include "mobilus/MockMqttMobilusGtwClient.hpp"

#include <gtest/gtest.h>

#include <unordered_map>
#include <vector>

using namespace jungi::mobilus_gtw_client;
using namespace mobmatter::driving_adapters::mobilus;
using mobmatter::application::model::MobilusDeviceId;
using mobmatter::application::model::MobilusDeviceType;
using mobmatter::common::logging::Logger;
using mobmatter::tests::mobilus::MockMqttMobilusGtwClient;

namespace {

class FakeMobilusDeviceNameHandler final : public MobilusDeviceNameHandler {
public:
    std::unordered_map<MobilusDeviceId, std::string> handledNames;

    void handle(MobilusDeviceId deviceId, const std::string& name)
    {
        handledNames[deviceId] = name;
    }

    bool supports(MobilusDeviceType deviceType) const { return MobilusDeviceType::Senso == deviceType; }
};

auto deviceListResponseStub()
{
    proto::DevicesListResponse response;

    auto devFoo = response.add_devices();
    devFoo->set_id(1);
    devFoo->set_name("Foo");
    devFoo->set_type(static_cast<google::protobuf::int32>(MobilusDeviceType::Cosmo));

    auto devBar = response.add_devices();
    devBar->set_id(2);
    devBar->set_name("Bar");
    devBar->set_type(static_cast<google::protobuf::int32>(MobilusDeviceType::Senso));

    return response;
}

}

TEST(MqttMobilusDeviceNameSyncerTest, Runs)
{
    MockMqttMobilusGtwClient client;
    MqttMobilusDeviceNameSyncer deviceNameSyncer(client, Logger::noop());
    FakeMobilusDeviceNameHandler handler;

    deviceNameSyncer.registerHandler(handler);
    client.mockResponse(std::make_unique<proto::DevicesListResponse>(deviceListResponseStub()));

    deviceNameSyncer.run();

    ASSERT_EQ(1, handler.handledNames.size());
    ASSERT_EQ("Bar", handler.handledNames[2]);
}
