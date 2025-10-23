#pragma once

#include "application/driven_ports/DeviceManagementService.h"
#include "common/logging/Logger.h"
#include "jungi/mobilus_gtw_client/MqttMobilusGtwClient.h"

namespace jungi::mobilus_gtw_client::proto {

class Device;

}

namespace mobmatter::driven_adapters::mobilus {

namespace model = mobmatter::application::model;
namespace logging = mobmatter::common::logging;
namespace proto = jungi::mobilus_gtw_client::proto;

class MqttMobilusDeviceManagementService final : public mobmatter::application::driven_ports::DeviceManagementService {
public:
    MqttMobilusDeviceManagementService(jungi::mobilus_gtw_client::MqttMobilusGtwClient& client, logging::Logger& logger);

    void renameDevice(model::MobilusDeviceId deviceId, std::string name) override;

private:
    jungi::mobilus_gtw_client::MqttMobilusGtwClient& mClient;
    logging::Logger& mLogger;

    bool patchDevice(model::MobilusDeviceId mobilusDeviceId, const proto::Device& device);
};

}
