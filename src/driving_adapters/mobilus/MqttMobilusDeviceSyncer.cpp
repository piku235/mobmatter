#include "MqttMobilusDeviceSyncer.h"
#include "HandlerResult.h"
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

void MqttMobilusDeviceSyncer::boot()
{
    proto::DevicesListResponse deviceList;

    if (!mClient.sendRequest(proto::DevicesListRequest(), deviceList)) {
        mLogger.error(LOG_TAG "Failed to get device list from mobilus");
        return;
    }

    proto::CurrentStateResponse currentStateResponse;

    if (!mClient.sendRequest(proto::CurrentStateRequest(), currentStateResponse)) {
        mLogger.error(LOG_TAG "Failed to get current state from mobilus");
        return;
    }

    std::unordered_map<MobilusDeviceId, const proto::Event*> currentStateMap;

    for (int i = 0; i < currentStateResponse.events_size(); i++) {
        auto& currentState = currentStateResponse.events(i);

        if (!currentState.has_device_id()) {
            // should not happen
            continue;
        }

        currentStateMap[currentState.device_id()] = &currentState;
    }

    for (int i = 0; i < deviceList.devices_size(); i++) {
        auto& device = deviceList.devices(i);

        if (!device.has_id()) {
            // should not happen
            continue;
        }

        auto it = currentStateMap.find(device.id());

        if (it == currentStateMap.end()) {
            mLogger.notice(LOG_TAG "Device: %s is missing current state, skipping [md=%" PRId64 "]", device.name().c_str(), device.id());
            continue;
        }

        HandlerResult r;

        for (MobilusDeviceSyncHandler& handler : mHandlers) {
            r = handler.handle(device, *it->second);
            if (HandlerResult::Unmatched != r) {
                break;
            }
        }

        if (HandlerResult::Unmatched == r) {
            mLogger.notice(LOG_TAG "Device: %s is not supported [md=%" PRId64 "]", device.name().c_str(), device.id());
        }
    }
}

}
