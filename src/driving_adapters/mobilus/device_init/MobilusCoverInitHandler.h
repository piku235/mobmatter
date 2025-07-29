#pragma once

#include "MobilusDeviceInitHandler.h"
#include "application/driven_ports/CoverRepository.h"
#include "application/driven_ports/EndpointIdGenerator.h"
#include "common/logging/Logger.h"

namespace mmbridge::driving_adapters::mobilus::device_init {

namespace driven_ports = mmbridge::application::driven_ports;
namespace logging = mmbridge::common::logging;

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
