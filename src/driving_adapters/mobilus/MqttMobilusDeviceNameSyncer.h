#pragma once

#include "MobilusDeviceNameHandler.h"
#include "common/logging/Logger.h"
#include "jungi/mobilus_gtw_client/MqttMobilusGtwClient.h"

#include <functional>
#include <vector>

namespace mobmatter::driving_adapters::mobilus {

namespace logging = mobmatter::common::logging;

class MqttMobilusDeviceNameSyncer final {
public:
    MqttMobilusDeviceNameSyncer(jungi::mobilus_gtw_client::MqttMobilusGtwClient& mobilusGtwClient, logging::Logger& logger);

    void registerHandler(MobilusDeviceNameHandler& handler);
    void run();

private:
    jungi::mobilus_gtw_client::MqttMobilusGtwClient& mMobilusGtwClient;
    std::vector<std::reference_wrapper<MobilusDeviceNameHandler>> mHandlers;
    logging::Logger& mLogger;
};

}
