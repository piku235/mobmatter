#pragma once

#include "application/driven_ports/CoverEndpointService.h"

namespace mobmatter::driven_adapters::matter::zcl {

namespace model = application::model;
namespace wc = application::model::window_covering;

class ZclCoverEndpointService final : public application::driven_ports::CoverEndpointService {
public:
    ZclCoverEndpointService(model::EndpointId aggregatorEndpointId);

    void addEndpoint(model::EndpointId endpointId, const wc::CoverSpecification& specification) override;
    void removeEndpoint(model::EndpointId endpointId) override;

private:
    model::EndpointId mAggregatorEndpointId;
};

}
