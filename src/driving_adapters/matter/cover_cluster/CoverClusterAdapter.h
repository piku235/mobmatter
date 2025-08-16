#pragma once

#include "BridgedDeviceBasicInfoAttributeAccess.h"
#include "CoverAttributeAccess.h"
#include "CoverCommandHandler.h"
#include "application/driven_ports/CoverRepository.h"
#include "common/logging/Logger.h"
#include "matter/AppComponent.h"

namespace mmbridge::driving_adapters::matter::cover_cluster {

namespace driven_ports = mmbridge::application::driven_ports;
namespace logging = mmbridge::common::logging;

class CoverClusterAdapter final : public mmbridge::matter::AppComponent {
public:
    CoverClusterAdapter(driven_ports::CoverRepository& coverRepository, logging::Logger& logger);
    ~CoverClusterAdapter();
    void boot() override;
    void shutdown() override;

private:
    CoverAttributeAccess mCoverAttributeAccess;
    CoverCommandHandler mCoverCommandHandler;
    BridgedDeviceBasicInfoAttributeAccess mBridgedDeviceBasicInfoAttributeAccess;
};

}
