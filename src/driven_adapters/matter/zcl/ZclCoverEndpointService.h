#pragma once

#include "application/driven_ports/CoverEndpointService.h"

namespace mmbridge::driven_adapters::matter::zcl {

namespace model = mmbridge::application::model;
namespace wc = mmbridge::application::model::window_covering;

class ZclCoverEndpointService final : public mmbridge::application::driven_ports::CoverEndpointService {
public:
    ZclCoverEndpointService(model::EndpointId aggregatorEndpointId);

    void addEndpoint(model::EndpointId endpointId, const wc::CoverSpecification& specification) override;
    void removeEndpoint(model::EndpointId endpointId) override;

private:
    model::EndpointId mAggregatorEndpointId;
};

}
