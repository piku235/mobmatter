#pragma once

#include "BridgedDeviceBasicInfoAttributeAccess.h"
#include "WindowCoveringAttributeAccess.h"
#include "WindowCoveringCommandHandler.h"
#include "application/driven_ports/CoverRepository.h"
#include "common/logging/Logger.h"
#include "matter/AppComponent.h"

namespace mmbridge::driving_adapters::matter::window_covering_cluster {

namespace driven_ports = mmbridge::application::driven_ports;
namespace logging = mmbridge::common::logging;

class WindowCoveringClusterAdapter final : public mmbridge::matter::AppComponent {
public:
    explicit WindowCoveringClusterAdapter(driven_ports::CoverRepository& coverRepository, logging::Logger& logger);
    ~WindowCoveringClusterAdapter();
    void boot() override;
    void shutdown() override;

private:
    WindowCoveringAttributeAccess mWindowCoveringAttributeAccess;
    WindowCoveringCommandHandler mWindowCoveringCommandHandler;
    BridgedDeviceBasicInfoAttributeAccess mBridgedDeviceBasicInfoAttributeAccess;
};

}
