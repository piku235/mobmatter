#pragma once

#include "MobilusEventHandler.h"
#include "common/logging/Logger.h"
#include "jungi/mobilus_gtw_client/MqttMobilusGtwClient.h"

namespace mobmatter::driving_adapters::mobilus {

namespace logging = mobmatter::common::logging;

class MqttMobilusDeviceStateSyncer final {
public:
    MqttMobilusDeviceStateSyncer(jungi::mobilus_gtw_client::MqttMobilusGtwClient& mobilusGtwClient, MobilusEventHandler& eventHandler, logging::Logger& logger);
    void run();

private:
    jungi::mobilus_gtw_client::MqttMobilusGtwClient& mMobilusGtwClient;
    MobilusEventHandler& mEventHandler;
    logging::Logger& mLogger;
};

}
