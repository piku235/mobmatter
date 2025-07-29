#pragma once

#include "application/driven_ports/MobilusCoverControlService.h"
#include "common/logging/Logger.h"
#include "jungi/mobilus_gtw_client/MqttMobilusGtwClient.h"

#include <string>

namespace jungi::mobilus_gtw_client::proto {

class CallEvents;

}

namespace mmbridge::driven_adapters::mobilus::cover_control {

namespace proto = jungi::mobilus_gtw_client::proto;
namespace model = mmbridge::application::model;
namespace logging = mmbridge::common::logging;

class MqttMobilusCoverControlService : public mmbridge::application::driven_ports::MobilusCoverControlService {
public:
    MqttMobilusCoverControlService(jungi::mobilus_gtw_client::MqttMobilusGtwClient& mobilusGtwClient, logging::Logger& logger);
    void liftCover(model::MobilusDeviceId mobilusDeviceId, model::window_covering::Position position) override;
    void stopCoverMotion(model::MobilusDeviceId mobilusDeviceId) override;

private:
    jungi::mobilus_gtw_client::MqttMobilusGtwClient& mMobilusGtwClient;
    logging::Logger& mLogger;

    proto::CallEvents callEventsFor(model::MobilusDeviceId mobilusDeviceId, const std::string& eventValue) const;
    std::string convertLiftPosition(model::window_covering::Position position) const;
};

}
