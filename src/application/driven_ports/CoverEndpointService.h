#pragma once

#include "application/model/EndpointId.h"
#include "application/model/window_covering/CoverSpecification.h"

namespace mobmatter::application::driven_ports {

namespace model = mobmatter::application::model;

class CoverEndpointService {
public:
    virtual ~CoverEndpointService() = default;
    virtual void addEndpoint(model::EndpointId endpointId, const model::window_covering::CoverSpecification& specification) = 0;
    virtual void removeEndpoint(model::EndpointId endpointId) = 0;
};

}
