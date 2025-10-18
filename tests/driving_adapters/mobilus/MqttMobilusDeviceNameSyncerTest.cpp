#include "driving_adapters/mobilus/MqttMobilusDeviceNameSyncer.h"
#include "FakeDeviceNameHandler.hpp"
#include "application/model/MobilusDeviceType.h"
#include "common/logging/Logger.h"
#include "jungi/mobilus_gtw_client/EventNumber.h"
#include "jungi/mobilus_gtw_client/proto/DevicesListResponse.pb.h"
#include "mobilus/MockMqttMobilusGtwClient.hpp"

#include <gtest/gtest.h>

using namespace jungi::mobilus_gtw_client;
using namespace mobmatter::driving_adapters::mobilus;
using mobmatter::application::model::MobilusDeviceId;
using mobmatter::application::model::MobilusDeviceType;
using mobmatter::common::logging::Logger;
using mobmatter::tests::mobilus::MockMqttMobilusGtwClient;

namespace {

auto deviceListResponseStub()
{
    proto::DevicesListResponse response;

    auto devCosmo = response.add_devices();
    devCosmo->set_id(1);
    devCosmo->set_name("Cosmo");
    devCosmo->set_type(static_cast<google::protobuf::int32>(MobilusDeviceType::Cosmo));

    auto devSenso = response.add_devices();
    devSenso->set_id(2);
    devSenso->set_name("Senso");
    devSenso->set_type(static_cast<google::protobuf::int32>(MobilusDeviceType::Senso));

    auto devSwitch = response.add_devices();
    devSwitch->set_id(2);
    devSwitch->set_name("Switch");
    devSwitch->set_type(static_cast<google::protobuf::int32>(MobilusDeviceType::Switch));

    return response;
}

}

TEST(MqttMobilusDeviceNameSyncerTest, Runs)
{
    MockMqttMobilusGtwClient client;
    MqttMobilusDeviceNameSyncer syncer(client, Logger::noop());
    FakeDeviceNameHandler handlerSenso(MobilusDeviceType::Senso);
    FakeDeviceNameHandler handlerCosmo(MobilusDeviceType::Cosmo);
    FakeDeviceNameHandler handlerUnmatched(MobilusDeviceType::SwitchNp);

    client.mockResponse(std::make_unique<proto::DevicesListResponse>(deviceListResponseStub()));
    syncer.registerHandler(handlerSenso);
    syncer.registerHandler(handlerCosmo);

    syncer.run();

    ASSERT_TRUE(handlerUnmatched.handledNames.empty());
    ASSERT_EQ(1, handlerSenso.handledNames.size());
    ASSERT_EQ(1, handlerCosmo.handledNames.size());
    ASSERT_EQ("Cosmo", handlerCosmo.handledNames[1]);
    ASSERT_EQ("Senso", handlerSenso.handledNames[2]);
}
