#pragma once

#include "application/driven_ports/EndpointIdGenerator.h"

namespace mmbridge::driven_adapters::persistence::in_memory {

namespace model = mmbridge::application::model;

class InMemoryEndpointIdGenerator final : public mmbridge::application::driven_ports::EndpointIdGenerator {
public:
    InMemoryEndpointIdGenerator(model::EndpointId initialEndpointId);
    model::EndpointId next() override;

private:
    model::EndpointId mNextEndpointId;
};

}
