#include "MqttMobilusDeviceStateSyncer.h"
#include "Log.h"
#include "jungi/mobilus_gtw_client/proto/CurrentStateRequest.pb.h"
#include "jungi/mobilus_gtw_client/proto/CurrentStateResponse.pb.h"

using namespace jungi::mobilus_gtw_client;

namespace mobmatter::driving_adapters::mobilus {

MqttMobilusDeviceStateSyncer::MqttMobilusDeviceStateSyncer(MqttMobilusGtwClient& mobilusGtwClient, MobilusDeviceEventHandler& eventHandler, logging::Logger& logger)
    : mMobilusGtwClient(mobilusGtwClient)
    , mEventHandler(eventHandler)
    , mLogger(logger)
{
}

void MqttMobilusDeviceStateSyncer::run()
{
    proto::CurrentStateResponse response;

    mLogger.notice(LOG_TAG "Syncing device states with mobilus");

    if (!mMobilusGtwClient.sendRequest(proto::CurrentStateRequest(), response)) {
        mLogger.error(LOG_TAG "Failed to sync device states with mobilus");
        return;
    }

    for (int i = 0; i < response.events_size(); i++) {
        mEventHandler.handle(response.events(i));
    }
}

}
