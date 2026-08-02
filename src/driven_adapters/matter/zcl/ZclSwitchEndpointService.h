#pragma once

#include "application/driven_ports/SwitchEndpointService.h"

namespace mobmatter::driven_adapters::matter::zcl {

namespace model = application::model;

class ZclSwitchEndpointService final : public application::driven_ports::SwitchEndpointService {
public:
    ZclSwitchEndpointService(model::EndpointId aggregatorEndpointId);

    void addEndpoint(model::EndpointId endpointId) override;
    void removeEndpoint(model::EndpointId endpointId) override;

private:
    model::EndpointId mAggregatorEndpointId;
};

}
