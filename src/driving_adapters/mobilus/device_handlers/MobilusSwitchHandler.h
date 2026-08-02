#pragma once

#include "application/driven_ports/EndpointIdGenerator.h"
#include "application/driven_ports/SwitchRepository.h"
#include "common/logging/Logger.h"
#include "driving_adapters/mobilus/MobilusDeviceEventHandler.h"
#include "driving_adapters/mobilus/MobilusDeviceSyncHandler.h"

namespace mobmatter::driving_adapters::mobilus::device_handlers {

namespace model = application::model;
namespace driven_ports = application::driven_ports;
namespace logging = common::logging;

class MobilusSwitchHandler final : public MobilusDeviceSyncHandler,
                                   public MobilusDeviceEventHandler {
public:
    MobilusSwitchHandler(driven_ports::SwitchRepository& switchRepository, driven_ports::EndpointIdGenerator& endpointIdGenerator, logging::Logger& logger);

    void sync(const DeviceStateMap& devices) override;
    Result handle(const proto::Event& event) override;

private:
    driven_ports::SwitchRepository& mSwitchRepository;
    driven_ports::EndpointIdGenerator& mEndpointIdGenerator;
    logging::Logger& mLogger;

    static std::optional<model::Switch::Error> parseError(const std::string& error);

    void init(const proto::Device& device, const proto::Event& lastEvent);
    bool apply(model::Switch& switch_, const proto::Device& device);
    bool apply(model::Switch& switch_, const proto::Event& event);
};

}
