#include "MqttMobilusDeviceManagementService.h"

#include <jungi/mobgtw/OperationType.h>
#include <jungi/mobgtw/proto/DevicesListRequest.pb.h>
#include <jungi/mobgtw/proto/DevicesListResponse.pb.h>
#include <jungi/mobgtw/proto/UpdateDeviceRequest.pb.h>
#include <jungi/mobgtw/proto/UpdateDeviceResponse.pb.h>

using namespace jungi::mobgtw;
using mobmatter::application::model::MobilusDeviceId;

#include <cinttypes>

#define LOG_TAG "GTW: "
#define LOG_SUFFIX " [md=%" PRId64 "]"

namespace mobmatter::driven_adapters::mobilus {

MqttMobilusDeviceManagementService::MqttMobilusDeviceManagementService(MqttMobilusGtwClient& client, logging::Logger& logger)
    : mClient(client)
    , mLogger(logger)
{
}

void MqttMobilusDeviceManagementService::renameDevice(MobilusDeviceId deviceId, std::string name)
{
    proto::Device device;
    device.set_name(std::move(name));

    if (patchDevice(deviceId, device)) {
        mLogger.notice(LOG_TAG "Renamed device to: %s" LOG_SUFFIX, device.name().c_str(), deviceId);
    }
}

bool MqttMobilusDeviceManagementService::patchDevice(model::MobilusDeviceId deviceId, const proto::Device& device)
{
    proto::DevicesListResponse deviceListResponse;

    if (!mClient.sendRequest(proto::DevicesListRequest(), deviceListResponse)) {
        mLogger.error(LOG_TAG "Failed to send device list request" LOG_SUFFIX, deviceId);
    }

    const proto::Device* matchedDevice = nullptr;

    for (auto& iteratedDevice : deviceListResponse.devices()) {
        if (deviceId == iteratedDevice.id()) {
            matchedDevice = &iteratedDevice;
            break;
        }
    }

    if (nullptr == matchedDevice) {
        mLogger.error(LOG_TAG "Device not found" LOG_SUFFIX, deviceId);
        return false;
    }

    proto::UpdateDeviceResponse updateDeviceResponse;
    proto::UpdateDeviceRequest updateDeviceRequest;

    updateDeviceRequest.set_operation_type(OperationType::Edit);
    updateDeviceRequest.mutable_device()->CopyFrom(*matchedDevice);
    updateDeviceRequest.mutable_device()->MergeFrom(device);

    if (!mClient.sendRequest(updateDeviceRequest, updateDeviceResponse)) {
        mLogger.error(LOG_TAG "Failed to send rename request" LOG_SUFFIX, deviceId);
        return false;
    }

    if (updateDeviceResponse.operation_status()) {
        mLogger.error(LOG_TAG "Failed to rename device: operation status %d" LOG_SUFFIX, updateDeviceResponse.operation_status(), deviceId);
        return false;
    }

    return true;
}

}
