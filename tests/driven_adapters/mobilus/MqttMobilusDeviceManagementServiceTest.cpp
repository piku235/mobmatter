#include "driven_adapters/mobilus/MqttMobilusDeviceManagementService.h"
#include "application/model/MobilusDeviceType.h"
#include "common/logging/Logger.h"
#include "mobilus/MockMqttMobilusGtwClient.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <jungi/mobgtw/OperationType.h>
#include <jungi/mobgtw/proto/DevicesListResponse.pb.h>
#include <jungi/mobgtw/proto/UpdateDeviceRequest.pb.h>
#include <jungi/mobgtw/proto/UpdateDeviceResponse.pb.h>

#include <memory>

using namespace jungi::mobgtw;
using mobmatter::application::model::MobilusDeviceType;
using mobmatter::common::logging::Logger;
using mobmatter::driven_adapters::mobilus::MqttMobilusDeviceManagementService;
using mobmatter::tests::mobilus::MockMqttMobilusGtwClient;
using testing::EndsWith;

namespace {

auto deviceListResponseStub()
{
    proto::DevicesListResponse response;

    auto device = response.add_devices();
    device->set_id(1);
    device->set_name("Foo");
    device->set_type(static_cast<google::protobuf::int32>(MobilusDeviceType::Senso));
    device->set_favourite(true);
    device->set_icon(10);
    device->set_inserttime(123456789);
    device->set_assigned_place_ids(123);
    device->add_assigned_group_ids(123);
    device->add_assigned_group_ids(234);

    auto other = response.add_devices();
    other->set_id(2);
    other->set_name("Another");

    return response;
}

auto updateDeviceResponseStub()
{
    proto::UpdateDeviceResponse response;
    response.set_operation_status(0);

    return response;
}

}

TEST(MqttMobilusDeviceManagementServiceTest, RenamesDevice)
{
    MockMqttMobilusGtwClient client;
    MqttMobilusDeviceManagementService deviceManagementService(client, Logger::noop());

    auto deviceList = deviceListResponseStub();
    auto& expectedDevice = deviceList.devices(0);

    client.mockResponse(std::make_unique<proto::DevicesListResponse>(deviceList));
    client.mockResponse(std::make_unique<proto::UpdateDeviceResponse>(updateDeviceResponseStub()));

    deviceManagementService.renameDevice(1, "Bar");

    ASSERT_EQ(2, client.sentRequests().size());
    ASSERT_THAT(client.sentRequests().back()->GetTypeName(), EndsWith("UpdateDeviceRequest"));

    auto request = static_cast<const proto::UpdateDeviceRequest*>(client.sentRequests().back().get());

    ASSERT_EQ(OperationType::Edit, request->operation_type());
    ASSERT_TRUE(request->has_device());

    ASSERT_EQ(1, request->device().id());
    ASSERT_EQ("Bar", request->device().name());
    ASSERT_EQ(expectedDevice.type(), request->device().type());
    ASSERT_TRUE(request->device().favourite());
    ASSERT_EQ(expectedDevice.icon(), request->device().icon());
    ASSERT_EQ(expectedDevice.inserttime(), request->device().inserttime());
    ASSERT_EQ(expectedDevice.assigned_place_ids(), request->device().assigned_place_ids());
    ASSERT_EQ(expectedDevice.assigned_group_ids_size(), request->device().assigned_group_ids_size());
    ASSERT_EQ(expectedDevice.assigned_group_ids(0), request->device().assigned_group_ids(0));
    ASSERT_EQ(expectedDevice.assigned_group_ids(1), request->device().assigned_group_ids(1));
}

TEST(MqttMobilusDeviceManagementServiceTest, RenameDeviceFails)
{
    MockMqttMobilusGtwClient client;
    MqttMobilusDeviceManagementService deviceManagementService(client, Logger::noop());

    deviceManagementService.renameDevice(1, "Bar");

    ASSERT_EQ(1, client.sentRequests().size());
    ASSERT_THAT(client.sentRequests().back()->GetTypeName(), EndsWith("DevicesListRequest"));
}

TEST(MqttMobilusDeviceManagementServiceTest, RenameDeviceFailsIfNotFound)
{
    MockMqttMobilusGtwClient client;
    MqttMobilusDeviceManagementService deviceManagementService(client, Logger::noop());

    client.mockResponse(std::make_unique<proto::DevicesListResponse>(deviceListResponseStub()));
    client.mockResponse(std::make_unique<proto::UpdateDeviceResponse>(updateDeviceResponseStub()));

    deviceManagementService.renameDevice(0, "Bar");

    ASSERT_EQ(1, client.sentRequests().size());
    ASSERT_THAT(client.sentRequests().front()->GetTypeName(), EndsWith("DevicesListRequest"));
}

TEST(MqttMobilusDeviceManagementServiceTest, RenameDeviceFailsIfUpdateDeviceResponseFailed)
{
    MockMqttMobilusGtwClient client;
    MqttMobilusDeviceManagementService deviceManagementService(client, Logger::noop());

    proto::UpdateDeviceResponse updateDeviceResponse;
    updateDeviceResponse.set_operation_status(1);

    client.mockResponse(std::make_unique<proto::DevicesListResponse>(deviceListResponseStub()));

    deviceManagementService.renameDevice(1, "Bar");

    ASSERT_EQ(2, client.sentRequests().size());
    ASSERT_THAT(client.sentRequests().front()->GetTypeName(), EndsWith("DevicesListRequest"));
    ASSERT_THAT(client.sentRequests().back()->GetTypeName(), EndsWith("UpdateDeviceRequest"));
}
