#pragma once

#include "MobilusEventHandler.h"
#include "application/driven_ports/CoverRepository.h"
#include "common/logging/Logger.h"

namespace mmbridge::driving_adapters::mobilus::device_events {

namespace driven_ports = mmbridge::application::driven_ports;
namespace logging = mmbridge::common::logging;

class MobilusCoverEventHandler : public MobilusEventHandler {
public:
    explicit MobilusCoverEventHandler(driven_ports::CoverRepository& coverRepository, logging::Logger& logger);
    Result handle(const proto::Event& event) override;

private:
    driven_ports::CoverRepository& mCoverRepository;
    logging::Logger& mLogger;
};

}
