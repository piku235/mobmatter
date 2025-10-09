#pragma once

#include "application/driven_ports/EndpointIdGenerator.h"

namespace mobmatter::tests::driven_adapters::persistence::in_memory {

namespace model = mobmatter::application::model;

class InMemoryEndpointIdGenerator final : public mobmatter::application::driven_ports::EndpointIdGenerator {
public:
    InMemoryEndpointIdGenerator(model::EndpointId initialEndpointId);
    std::optional<model::EndpointId> next() override;

private:
    model::EndpointId mNextEndpointId;
};

}
