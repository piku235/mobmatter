#pragma once

#include "application/model/EndpointId.h"

#include <optional>

namespace mobmatter::application::driven_ports {

class EndpointIdGenerator {
public:
    virtual ~EndpointIdGenerator() = default;
    virtual std::optional<model::EndpointId> next() = 0;
};

}
