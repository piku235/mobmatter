#pragma once

#include "application/driven_ports/CoverControlService.h"
#include "common/logging/Logger.h"

#include <jungi/mobgtw/MqttMobilusGtwClient.h>

#include <string>

namespace jungi::mobgtw::proto {

class CallEvents;

}

namespace mobmatter::driven_adapters::mobilus {

namespace proto = jungi::mobgtw::proto;
namespace model = application::model;
namespace logging = common::logging;

class MqttMobilusCoverControlService : public application::driven_ports::CoverControlService {
public:
    MqttMobilusCoverControlService(jungi::mobgtw::MqttMobilusGtwClient& client, logging::Logger& logger);

    void openCover(model::MobilusDeviceId deviceId) override;
    void closeCover(model::MobilusDeviceId deviceId) override;
    void liftCover(model::MobilusDeviceId deviceId, model::window_covering::Position position) override;
    void tiltCover(model::MobilusDeviceId deviceId, model::window_covering::Position position) override;
    void stopCoverMotion(model::MobilusDeviceId deviceId) override;

private:
    jungi::mobgtw::MqttMobilusGtwClient& mClient;
    logging::Logger& mLogger;

    static std::string convertLiftPosition(model::window_covering::Position position);
    static std::string convertTiltPosition(model::window_covering::Position position);
    static proto::CallEvents callEventsFor(model::MobilusDeviceId deviceId, const std::string& eventValue);
};

}
