#pragma once

#include "application/driven_ports/MobilusCoverControlService.h"
#include "common/logging/Logger.h"
#include "jungi/mobilus_gtw_client/MqttMobilusGtwClient.h"

#include <string>

namespace jungi::mobilus_gtw_client::proto {

class CallEvents;

}

namespace mobmatter::driven_adapters::mobilus {

namespace proto = jungi::mobilus_gtw_client::proto;
namespace model = mobmatter::application::model;
namespace logging = mobmatter::common::logging;

class MqttMobilusCoverControlService : public mobmatter::application::driven_ports::MobilusCoverControlService {
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
