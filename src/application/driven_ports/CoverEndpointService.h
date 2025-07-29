#pragma once

#include "application/model/window_covering/Cover.h"

namespace mmbridge::application::driven_ports {

namespace model = mmbridge::application::model;
namespace wc = mmbridge::application::model::window_covering;

class CoverEndpointService {
public:
    virtual ~CoverEndpointService() = default;
    virtual void addEndpoint(model::EndpointId endpointId, const wc::CoverSpecification& specification) = 0;
    virtual void removeEndpoint(model::EndpointId endpointId) = 0;
};

}
