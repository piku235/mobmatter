#pragma once

#include "application/model/EndpointId.h"

namespace mobmatter::application::driven_ports {

namespace model = mobmatter::application::model;

class EndpointIdGenerator {
public:
    virtual ~EndpointIdGenerator() = default;
    virtual model::EndpointId next() = 0;
};

}
