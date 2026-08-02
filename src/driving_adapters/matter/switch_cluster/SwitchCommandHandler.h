#pragma once

#include "application/driven_ports/SwitchRepository.h"
#include "common/logging/Logger.h"

#include <app/CommandHandlerInterface.h>

namespace mobmatter::driving_adapters::matter::switch_cluster {

namespace driven_ports = application::driven_ports;
namespace logging = common::logging;

class SwitchCommandHandler final : public chip::app::CommandHandlerInterface {
public:
    SwitchCommandHandler(driven_ports::SwitchRepository& switchRepository, logging::Logger& logger);
    void InvokeCommand(HandlerContext& handlerContext) override;

private:
    driven_ports::SwitchRepository& mSwitchRepository;
    logging::Logger& mLogger;
};

}
