#pragma once

#include "MobilusDeviceInfoHandler.h"
#include "common/logging/Logger.h"
#include "jungi/mobilus_gtw_client/MqttMobilusGtwClient.h"

#include <functional>
#include <vector>

namespace mobmatter::driving_adapters::mobilus {

namespace logging = mobmatter::common::logging;

class MqttMobilusDeviceInfoSyncer final {
public:
    MqttMobilusDeviceInfoSyncer(jungi::mobilus_gtw_client::MqttMobilusGtwClient& client, logging::Logger& logger);

    void registerHandler(MobilusDeviceInfoHandler& handler);
    void run();

private:
    jungi::mobilus_gtw_client::MqttMobilusGtwClient& mClient;
    std::vector<std::reference_wrapper<MobilusDeviceInfoHandler>> mHandlers;
    logging::Logger& mLogger;
};

}
