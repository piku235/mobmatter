#pragma once

#include "SwitchAttributeAccess.h"
#include "SwitchCommandHandler.h"
#include "application/driven_ports/SwitchRepository.h"
#include "common/logging/Logger.h"
#include "matter/AppComponent.h"

namespace mobmatter::driving_adapters::matter::switch_cluster {

namespace driven_ports = application::driven_ports;
namespace logging = common::logging;

class SwitchClusterAdapter final : public mobmatter::matter::AppComponent {
public:
    SwitchClusterAdapter(driven_ports::SwitchRepository& switchRepository, logging::Logger& logger);

    void boot() override;
    void shutdown() override;

private:
    SwitchAttributeAccess mSwitchAttributeAccess;
    SwitchCommandHandler mSwitchCommandHandler;
};

}
