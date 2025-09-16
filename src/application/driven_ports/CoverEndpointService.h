#pragma once

#include "application/model/window_covering/Cover.h"

namespace mobmatter::application::driven_ports {

namespace model = mobmatter::application::model;
namespace wc = mobmatter::application::model::window_covering;

class CoverEndpointService {
public:
    virtual ~CoverEndpointService() = default;
    virtual void addEndpoint(model::EndpointId endpointId, const wc::CoverSpecification& specification) = 0;
    virtual void removeEndpoint(model::EndpointId endpointId) = 0;
};

}
