#pragma once

#include "AppComponent.h"
#include "application/driven_ports/CoverEndpointService.h"
#include "application/driven_ports/CoverRepository.h"
#include "common/logging/Logger.h"

namespace mobmatter::matter {

namespace driven_ports = mobmatter::application::driven_ports;
namespace logging = mobmatter::common::logging;

class DeviceEndpointLoader final : public AppComponent {
public:
    DeviceEndpointLoader(driven_ports::CoverRepository& coveRepository, driven_ports::CoverEndpointService& coverEndpointService, logging::Logger& logger)
        : mCoveRepository(coveRepository)
        , mCoverEndpointService(coverEndpointService)
        , mLogger(logger)
    {
    }

    void boot()
    {
        for (auto& cover : mCoveRepository.all()) {
            mCoverEndpointService.addEndpoint(cover.endpointId(), cover.specification());
            mLogger.notice("Loaded cover at endpoint: %u", cover.endpointId());
        }
    }

private:
    driven_ports::CoverRepository& mCoveRepository;
    driven_ports::CoverEndpointService& mCoverEndpointService;
    logging::Logger& mLogger;
};

}
