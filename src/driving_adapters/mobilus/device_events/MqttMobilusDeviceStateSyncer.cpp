#include "MqttMobilusDeviceStateSyncer.h"
#include "jungi/mobilus_gtw_client/proto/CurrentStateRequest.pb.h"
#include "jungi/mobilus_gtw_client/proto/CurrentStateResponse.pb.h"

using namespace jungi::mobilus_gtw_client;

namespace mmbridge::driving_adapters::mobilus::device_events {

MqttMobilusDeviceStateSyncer::MqttMobilusDeviceStateSyncer(MqttMobilusGtwClient& mobilusGtwClient, MobilusEventHandler& eventHandler, logging::Logger& logger)
    : mMobilusGtwClient(mobilusGtwClient)
    , mEventHandler(eventHandler)
    , mLogger(logger)
{
}

void MqttMobilusDeviceStateSyncer::run()
{
    proto::CurrentStateResponse response;

    mLogger.notice("Syncing device states with mobilus");

    if (auto e = mMobilusGtwClient.sendRequest(proto::CurrentStateRequest(), response); !e) {
        mLogger.error("Failed to sync device states with mobilus: %s", e.error().message.c_str());
        return;
    }

    for (int i = 0; i < response.events_size(); i++) {
        mEventHandler.handle(response.events(i));
    }
}

}
