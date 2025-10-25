#pragma once

#include "application/driven_ports/CoverRepository.h"
#include "application/driven_ports/EndpointIdGenerator.h"
#include "application/model/window_covering/Cover.h"
#include "application/model/window_covering/CoverSpecification.h"
#include "application/model/window_covering/Position.h"
#include "common/logging/Logger.h"
#include "driving_adapters/mobilus/MobilusDeviceEventHandler.h"
#include "driving_adapters/mobilus/MobilusDeviceSyncHandler.h"

#include <optional>

namespace mobmatter::driving_adapters::mobilus::device_handlers {

namespace model = mobmatter::application::model;
namespace driven_ports = mobmatter::application::driven_ports;
namespace logging = mobmatter::common::logging;

class MobilusCoverHandler final : public MobilusDeviceSyncHandler,
                                  public MobilusDeviceEventHandler {
public:
    MobilusCoverHandler(driven_ports::CoverRepository& coverRepository, driven_ports::EndpointIdGenerator& endpointIdGenerator, logging::Logger& logger);

    HandlerResult handle(const proto::Device& deviceInfo, const proto::Event& lastEvent) override;
    HandlerResult handle(const proto::Event& event) override;

private:
    driven_ports::CoverRepository& mCoverRepository;
    driven_ports::EndpointIdGenerator& mEndpointIdGenerator;
    logging::Logger& mLogger;

    void init(model::window_covering::CoverSpecification coverSpec, const proto::Device& device, const proto::Event& lastEvent);
    void apply(model::window_covering::Cover& cover, const proto::Device& deviceInfo);
    void apply(model::window_covering::Cover& cover, const proto::Event& event);
    std::optional<model::window_covering::Position> convertLiftPosition(const std::string& value);
};

}
