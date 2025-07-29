#pragma once

#include "application/model/EndpointId.h"

namespace mmbridge::application::driven_ports {

namespace model = mmbridge::application::model;

class EndpointIdGenerator {
public:
    virtual ~EndpointIdGenerator() = default;
    virtual model::EndpointId next() = 0;
};

}
