#pragma once

#include "MobilusDeviceInitHandler.h"
#include "application/model/MobilusDeviceId.h"
#include "application/model/MobilusDeviceType.h"
#include "common/logging/Logger.h"
#include "jungi/mobilus_gtw_client/MqttMobilusGtwClient.h"

#include <memory>
#include <optional>
#include <vector>

namespace mmbridge::driving_adapters::mobilus::device_init {

namespace model = mmbridge::application::model;
namespace logging = mmbridge::common::logging;

class MqttMobilusDeviceInitializer final {
public:
    MqttMobilusDeviceInitializer(jungi::mobilus_gtw_client::MqttMobilusGtwClient& mobilusGtwClient, logging::Logger& logger);
    void useTestDeviceOnly(model::MobilusDeviceId deviceId);
    void registerHandler(std::unique_ptr<MobilusDeviceInitHandler> handler);
    bool run();

private:
    jungi::mobilus_gtw_client::MqttMobilusGtwClient& mMobilusGtwClient;
    std::optional<model::MobilusDeviceId> mTestDeviceId;
    logging::Logger& mLogger;
    std::vector<std::unique_ptr<MobilusDeviceInitHandler>> mHandlers;

    MobilusDeviceInitHandler* handlerFor(model::MobilusDeviceType deviceType);
};

}
