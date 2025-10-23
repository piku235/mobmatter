#include "MqttMobilusDeviceInfoSyncer.h"
#include "Log.h"
#include "jungi/mobilus_gtw_client/proto/DevicesListRequest.pb.h"
#include "jungi/mobilus_gtw_client/proto/DevicesListResponse.pb.h"

using namespace jungi::mobilus_gtw_client;

namespace mobmatter::driving_adapters::mobilus {

MqttMobilusDeviceInfoSyncer::MqttMobilusDeviceInfoSyncer(MqttMobilusGtwClient& client, logging::Logger& logger)
    : mClient(client)
    , mLogger(logger)
{
}

void MqttMobilusDeviceInfoSyncer::registerHandler(MobilusDeviceInfoHandler& handler)
{
    mHandlers.push_back(handler);
}

void MqttMobilusDeviceInfoSyncer::run()
{
    proto::DevicesListResponse response;

    if (!mClient.sendRequest(proto::DevicesListRequest(), response)) {
        mLogger.error(LOG_TAG "Failed to sync device names with mobilus");
        return;
    }

    for (int i = 0; i < response.devices_size(); i++) {
        auto& device = response.devices(i);

        for (MobilusDeviceInfoHandler& handler : mHandlers) {
            if (HandlerResult::Unmatched != handler.handle(device)) {
                break;
            }
        }
    }
}

}
