#pragma once

#include "MobilusDeviceEventHandler.h"
#include "common/logging/Logger.h"
#include "jungi/mobilus_gtw_client/MqttMobilusGtwClient.h"

namespace mobmatter::driving_adapters::mobilus {

namespace logging = mobmatter::common::logging;

class MqttMobilusDeviceStateSyncer final {
public:
    MqttMobilusDeviceStateSyncer(jungi::mobilus_gtw_client::MqttMobilusGtwClient& mobilusGtwClient, MobilusDeviceEventHandler& eventHandler, logging::Logger& logger);
    void run();

private:
    jungi::mobilus_gtw_client::MqttMobilusGtwClient& mMobilusGtwClient;
    MobilusDeviceEventHandler& mEventHandler;
    logging::Logger& mLogger;
};

}
