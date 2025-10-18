#pragma once

#include "application/driven_ports/CoverRepository.h"
#include "common/logging/Logger.h"
#include "driving_adapters/mobilus/MobilusDeviceNameHandler.h"

namespace mobmatter::driving_adapters::mobilus::cover {

namespace driven_ports = mobmatter::application::driven_ports;
namespace logging = mobmatter::common::logging;

class MobilusCoverNameHandler : public MobilusDeviceNameHandler {
public:
    MobilusCoverNameHandler(driven_ports::CoverRepository& coverRepository, logging::Logger& logger);
    
    void handle(model::MobilusDeviceId deviceId, const std::string& name) override;
    bool supports(model::MobilusDeviceType deviceType) const override;

private:
    driven_ports::CoverRepository& mCoverRepository;
    logging::Logger& mLogger;
};

}
