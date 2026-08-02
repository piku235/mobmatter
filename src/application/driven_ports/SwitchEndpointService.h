#pragma once

#include "application/model/EndpointId.h"

namespace mobmatter::application::driven_ports {

class SwitchEndpointService {
public:
    virtual ~SwitchEndpointService() = default;
    virtual void addEndpoint(model::EndpointId endpointId) = 0;
    virtual void removeEndpoint(model::EndpointId endpointId) = 0;
};

}
