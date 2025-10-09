#include "InMemoryEndpointIdGenerator.h"

using mobmatter::application::model::EndpointId;

namespace mobmatter::tests::driven_adapters::persistence::in_memory {

InMemoryEndpointIdGenerator::InMemoryEndpointIdGenerator(EndpointId initialEndpointId)
    : mNextEndpointId(initialEndpointId)
{
}

std::optional<model::EndpointId> InMemoryEndpointIdGenerator::next()
{
    return mNextEndpointId++;
}

}
