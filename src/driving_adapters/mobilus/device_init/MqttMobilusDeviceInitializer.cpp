#include "MqttMobilusDeviceInitializer.h"
#include "application/model/MobilusDeviceType.h"
#include "jungi/mobilus_gtw_client/proto/CurrentStateRequest.pb.h"
#include "jungi/mobilus_gtw_client/proto/CurrentStateResponse.pb.h"
#include "jungi/mobilus_gtw_client/proto/DevicesListRequest.pb.h"
#include "jungi/mobilus_gtw_client/proto/DevicesListResponse.pb.h"

#include <cinttypes>
#include <unordered_map>

using namespace jungi::mobilus_gtw_client;
using namespace mmbridge::application::model;

namespace mmbridge::driving_adapters::mobilus::device_init {

MqttMobilusDeviceInitializer::MqttMobilusDeviceInitializer(MqttMobilusGtwClient& mobilusGtwClient, logging::Logger& logger)
    : mMobilusGtwClient(mobilusGtwClient)
    , mLogger(logger)
{
}

void MqttMobilusDeviceInitializer::useTestDeviceOnly(MobilusDeviceId deviceId)
{
    mTestDeviceId = deviceId;
}

void MqttMobilusDeviceInitializer::registerHandler(std::unique_ptr<MobilusDeviceInitHandler> handler)
{
    mHandlers.push_back(std::move(handler));
}

bool MqttMobilusDeviceInitializer::run()
{
    proto::DevicesListResponse deviceList;

    if (auto e = mMobilusGtwClient.sendRequest(proto::DevicesListRequest(), deviceList); !e) {
        mLogger.error("Failed to get device list from mobilus: %s", e.error().message.c_str());
        return false;
    }

    proto::CurrentStateResponse currentStateResponse;

    if (auto e = mMobilusGtwClient.sendRequest(proto::CurrentStateRequest(), currentStateResponse); !e) {
        mLogger.error("Failed to get current state from mobilus: %s", e.error().message.c_str());
        return false;
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

        if (mTestDeviceId && *mTestDeviceId != device.id()) {
            continue;
        }

        auto deviceType = static_cast<MobilusDeviceType>(device.type());
        auto handler = handlerFor(deviceType);

        if (nullptr == handler) {
            mLogger.notice("Device: %s is not supported [md=%" PRId64 "]", device.name().c_str(), device.id());
            continue;
        }

        auto it = currentStateMap.find(device.id());

        if (it == currentStateMap.end()) {
            mLogger.notice("Device: %s is missing current state, skipping [md=%" PRId64 "]", device.name().c_str(), device.id());
            continue;
        }

        handler->initDevice(device, *it->second);
    }

    return true;
}

MobilusDeviceInitHandler* MqttMobilusDeviceInitializer::handlerFor(model::MobilusDeviceType deviceType)
{
    for (auto& handler : mHandlers) {
        if (handler->supports(deviceType)) {
            return handler.get();
        }
    }

    return nullptr;
}

}
