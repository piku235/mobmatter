#pragma once

#include "AppComponent.h"
#include "application/driven_ports/CoverEndpointService.h"
#include "application/driven_ports/CoverRepository.h"
#include "application/driven_ports/SwitchEndpointService.h"
#include "application/driven_ports/SwitchRepository.h"
#include "common/logging/Logger.h"

namespace mobmatter::matter {

namespace driven_ports = application::driven_ports;
namespace logging = common::logging;

class DeviceEndpointLoader final : public AppComponent {
public:
    DeviceEndpointLoader(driven_ports::CoverRepository& coveRepository, driven_ports::SwitchRepository& switchRepository, driven_ports::CoverEndpointService& coverEndpointService, driven_ports::SwitchEndpointService& switchEndpointService, logging::Logger& logger)
        : mCoveRepository(coveRepository)
        , mSwitchRepository(switchRepository)
        , mCoverEndpointService(coverEndpointService)
        , mSwitchEndpointService(switchEndpointService)
        , mLogger(logger)
    {
    }

    void run() override
    {
        for (auto& cover : mCoveRepository.all()) {
            mCoverEndpointService.addEndpoint(cover.endpointId(), cover.specification());
            mLogger.notice("Loaded cover at endpoint: %u", cover.endpointId());
        }
        for (auto& switch_ : mSwitchRepository.all()) {
            mSwitchEndpointService.addEndpoint(switch_.endpointId());
            mLogger.notice("Loaded switch at endpoint: %u", switch_.endpointId());
        }
    }

private:
    driven_ports::CoverRepository& mCoveRepository;
    driven_ports::SwitchRepository& mSwitchRepository;
    driven_ports::CoverEndpointService& mCoverEndpointService;
    driven_ports::SwitchEndpointService& mSwitchEndpointService;
    logging::Logger& mLogger;
};

}
