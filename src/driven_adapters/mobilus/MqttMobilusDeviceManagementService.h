#pragma once

#include "application/driven_ports/DeviceManagementService.h"
#include "common/logging/Logger.h"
#include "jungi/mobgtw/MqttMobilusGtwClient.h"

namespace jungi::mobgtw::proto {

class Device;

}

namespace mobmatter::driven_adapters::mobilus {

namespace model = mobmatter::application::model;
namespace logging = mobmatter::common::logging;
namespace proto = jungi::mobgtw::proto;

class MqttMobilusDeviceManagementService final : public mobmatter::application::driven_ports::DeviceManagementService {
public:
    MqttMobilusDeviceManagementService(jungi::mobgtw::MqttMobilusGtwClient& client, logging::Logger& logger);

    void renameDevice(model::MobilusDeviceId deviceId, std::string name) override;

private:
    jungi::mobgtw::MqttMobilusGtwClient& mClient;
    logging::Logger& mLogger;

    bool patchDevice(model::MobilusDeviceId mobilusDeviceId, const proto::Device& device);
};

}
