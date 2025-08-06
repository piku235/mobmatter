#pragma once

#include "driving_adapters/mobilus/MobilusEventHandler.h"
#include "application/driven_ports/CoverRepository.h"
#include "common/logging/Logger.h"

namespace mmbridge::driving_adapters::mobilus::cover {

namespace driven_ports = mmbridge::application::driven_ports;
namespace logging = mmbridge::common::logging;

class MobilusCoverEventHandler : public MobilusEventHandler {
public:
    MobilusCoverEventHandler(driven_ports::CoverRepository& coverRepository, logging::Logger& logger);
    Result handle(const proto::Event& event) override;

private:
    driven_ports::CoverRepository& mCoverRepository;
    logging::Logger& mLogger;
};

}
