#include "driving_adapters/mobilus/MqttMobilusDeviceInfoSyncer.h"
#include "FakeDeviceInfoHandler.hpp"
#include "application/model/MobilusDeviceType.h"
#include "common/logging/Logger.h"
#include "driving_adapters/mobilus/HandlerResult.h"
#include "jungi/mobilus_gtw_client/EventNumber.h"
#include "jungi/mobilus_gtw_client/proto/DevicesListResponse.pb.h"
#include "mobilus/MockMqttMobilusGtwClient.hpp"

#include <gtest/gtest.h>

using namespace jungi::mobilus_gtw_client;
using namespace mobmatter::driving_adapters::mobilus;
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

    return response;
}

}

TEST(MqttMobilusDeviceInfoSyncerTest, DoesNotDelegateDeviceInfoIfNoHandlers)
{
    MockMqttMobilusGtwClient client;
    MqttMobilusDeviceInfoSyncer syncer(client, Logger::noop());

    client.mockResponse(std::make_unique<proto::DevicesListResponse>(deviceListResponseStub()));

    syncer.run();
}

TEST(MqttMobilusDeviceInfoSyncerTest, DelegatesDeviceInfoToHandler)
{
    MockMqttMobilusGtwClient client;
    MqttMobilusDeviceInfoSyncer syncer(client, Logger::noop());

    FakeDeviceInfoHandler handlerSenso(HandlerResult::Unmatched);
    FakeDeviceInfoHandler handlerCosmo(HandlerResult::Handled);
    FakeDeviceInfoHandler handlerOther(HandlerResult::Unmatched);

    client.mockResponse(std::make_unique<proto::DevicesListResponse>(deviceListResponseStub()));
    syncer.registerHandler(handlerSenso);
    syncer.registerHandler(handlerCosmo);
    syncer.registerHandler(handlerOther);

    syncer.run();

    ASSERT_EQ(2, handlerSenso.seenDevices().size());
    ASSERT_EQ(2, handlerCosmo.seenDevices().size());
    ASSERT_TRUE(handlerOther.seenDevices().empty());
}
