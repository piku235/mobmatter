#pragma once

#include "application/driven_ports/CoverRepository.h"
#include "application/driven_ports/EndpointIdGenerator.h"
#include "common/logging/Logger.h"
#include "driving_adapters/mobilus/MobilusDeviceInitHandler.h"

namespace mobmatter::driving_adapters::mobilus::cover {

namespace driven_ports = mobmatter::application::driven_ports;
namespace logging = mobmatter::common::logging;

class MobilusCoverInitHandler final : public MobilusDeviceInitHandler {
public:
    MobilusCoverInitHandler(driven_ports::CoverRepository& coverRepository, driven_ports::EndpointIdGenerator& endpointIdGenerator, logging::Logger& logger);

    void initDevice(const proto::Device& device, const proto::Event& currentState) override;
    bool supports(model::MobilusDeviceType deviceType) override;

private:
    driven_ports::CoverRepository& mCoverRepository;
    driven_ports::EndpointIdGenerator& mEndpointIdGenerator;
    logging::Logger& mLogger;
};

}
