#pragma once

#include "MobilusEventHandler.h"
#include "common/logging/Logger.h"
#include "jungi/mobilus_gtw_client/MqttMobilusGtwClient.h"
#include "matter/AppComponent.h"

namespace mmbridge::driving_adapters::mobilus::device_events {

namespace logging = mmbridge::common::logging;

class MqttMobilusDeviceStateSyncer final : public mmbridge::matter::AppComponent {
public:
    MqttMobilusDeviceStateSyncer(jungi::mobilus_gtw_client::MqttMobilusGtwClient& mobilusGtwClient, MobilusEventHandler& eventHandler, logging::Logger& logger);
    void run() override;

private:
    jungi::mobilus_gtw_client::MqttMobilusGtwClient& mMobilusGtwClient;
    MobilusEventHandler& mEventHandler;
    logging::Logger& mLogger;
};

}
