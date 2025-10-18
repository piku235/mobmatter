#include "MqttMobilusDeviceNameSyncer.h"
#include "Log.h"
#include "application/model/MobilusDeviceType.h"
#include "jungi/mobilus_gtw_client/proto/DevicesListRequest.pb.h"
#include "jungi/mobilus_gtw_client/proto/DevicesListResponse.pb.h"

using namespace jungi::mobilus_gtw_client;
using mobmatter::application::model::MobilusDeviceType;

namespace mobmatter::driving_adapters::mobilus {

MqttMobilusDeviceNameSyncer::MqttMobilusDeviceNameSyncer(MqttMobilusGtwClient& mobilusGtwClient, logging::Logger& logger)
    : mMobilusGtwClient(mobilusGtwClient)
    , mLogger(logger)
{
}

void MqttMobilusDeviceNameSyncer::registerHandler(MobilusDeviceNameHandler& handler)
{
    mHandlers.push_back(handler);
}

void MqttMobilusDeviceNameSyncer::run()
{
    proto::DevicesListResponse response;

    mLogger.notice(LOG_TAG "Syncing device names with mobilus");

    if (!mMobilusGtwClient.sendRequest(proto::DevicesListRequest(), response)) {
        mLogger.error(LOG_TAG "Failed to sync device names with mobilus");
        return;
    }

    for (int i = 0; i < response.devices_size(); i++) {
        auto& device = response.devices(i);
        for (MobilusDeviceNameHandler& handler : mHandlers) {
            if (!handler.supports(static_cast<MobilusDeviceType>(device.type()))) {
                continue;
            }

            handler.handle(device.id(), device.name());
        }
    }
}

}
