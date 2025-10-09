#pragma once

#include "application/model/EndpointId.h"

#include <optional>

namespace mobmatter::application::driven_ports {

namespace model = mobmatter::application::model;

class EndpointIdGenerator {
public:
    virtual ~EndpointIdGenerator() = default;
    virtual std::optional<model::EndpointId> next() = 0;
};

}
