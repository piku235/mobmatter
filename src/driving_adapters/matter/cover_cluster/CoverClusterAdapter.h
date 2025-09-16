#pragma once

#include "BridgedDeviceBasicInfoAttributeAccess.h"
#include "CoverAttributeAccess.h"
#include "CoverCommandHandler.h"
#include "application/driven_ports/CoverRepository.h"
#include "common/logging/Logger.h"
#include "matter/AppComponent.h"

namespace mobmatter::driving_adapters::matter::cover_cluster {

namespace driven_ports = mobmatter::application::driven_ports;
namespace logging = mobmatter::common::logging;

class CoverClusterAdapter final : public mobmatter::matter::AppComponent {
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
