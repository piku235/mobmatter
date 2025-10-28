#include "MqttMobilusDeviceSyncer.h"
#include "DeviceStateMap.h"
#include "Log.h"
#include "application/model/MobilusDeviceId.h"
#include "jungi/mobilus_gtw_client/proto/CurrentStateRequest.pb.h"
#include "jungi/mobilus_gtw_client/proto/CurrentStateResponse.pb.h"
#include "jungi/mobilus_gtw_client/proto/DevicesListRequest.pb.h"
#include "jungi/mobilus_gtw_client/proto/DevicesListResponse.pb.h"

#include <unordered_map>

using namespace jungi::mobilus_gtw_client;
using namespace mobmatter::application::model;

namespace mobmatter::driving_adapters::mobilus {

MqttMobilusDeviceSyncer::MqttMobilusDeviceSyncer(MqttMobilusGtwClient& client, logging::Logger& logger)
    : mClient(client)
    , mLogger(logger)
{
}

void MqttMobilusDeviceSyncer::registerHandler(MobilusDeviceSyncHandler& handler)
{
    mHandlers.push_back(handler);
}

void MqttMobilusDeviceSyncer::run()
{
    proto::DevicesListResponse deviceList;

    mLogger.info(LOG_TAG "Syncing devices");

    if (!mClient.sendRequest(proto::DevicesListRequest(), deviceList)) {
        mLogger.error(LOG_TAG "Failed to get device list from mobilus");
        return;
    }

    proto::CurrentStateResponse currentStateResponse;

    if (!mClient.sendRequest(proto::CurrentStateRequest(), currentStateResponse)) {
        mLogger.error(LOG_TAG "Failed to get current state from mobilus");
        return;
    }

    DeviceStateMap devices;
    devices.reserve(deviceList.devices_size());

    for (int i = 0; i < deviceList.devices_size(); i++) {
        auto& device = deviceList.devices(i);

        if (device.has_id()) {
            devices[device.id()].device.CheckTypeAndMergeFrom(device);
        }
    }

    for (int i = 0; i < currentStateResponse.events_size(); i++) {
        auto& lastEvent = currentStateResponse.events(i);

        if (lastEvent.has_device_id()) {
            devices[lastEvent.device_id()].lastEvent.CheckTypeAndMergeFrom(lastEvent);
        }
    }

    for (MobilusDeviceSyncHandler& handler : mHandlers) {
        handler.sync(devices);
    }
}

}
