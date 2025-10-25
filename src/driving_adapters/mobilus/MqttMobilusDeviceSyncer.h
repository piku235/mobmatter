#pragma once

#include "MobilusDeviceSyncHandler.h"
#include "common/logging/Logger.h"
#include "jungi/mobilus_gtw_client/MqttMobilusGtwClient.h"
#include "matter/AppComponent.h"

#include <functional>
#include <vector>

namespace mobmatter::driving_adapters::mobilus {

namespace logging = mobmatter::common::logging;

class MqttMobilusDeviceSyncer final : public mobmatter::matter::AppComponent {
public:
    MqttMobilusDeviceSyncer(jungi::mobilus_gtw_client::MqttMobilusGtwClient& client, logging::Logger& logger);

    void registerHandler(MobilusDeviceSyncHandler& handler);
    void run() override;

private:
    jungi::mobilus_gtw_client::MqttMobilusGtwClient& mClient;
    std::vector<std::reference_wrapper<MobilusDeviceSyncHandler>> mHandlers;
    logging::Logger& mLogger;
};

}
