#pragma once

#include "application/driven_ports/CoverRepository.h"
#include "common/logging/Logger.h"

#include <app/CommandHandlerInterface.h>

namespace mmbridge::driving_adapters::matter::cover_cluster {

namespace driven_ports = mmbridge::application::driven_ports;
namespace logging = mmbridge::common::logging;

class CoverCommandHandler final : public chip::app::CommandHandlerInterface {
public:
    CoverCommandHandler(driven_ports::CoverRepository& coverRepository, logging::Logger& logger);
    void InvokeCommand(HandlerContext& handlerContext) override;

private:
    driven_ports::CoverRepository& mCoverRepository;
    logging::Logger& mLogger;
};

}
