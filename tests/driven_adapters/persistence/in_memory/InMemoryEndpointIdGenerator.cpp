#include "InMemoryEndpointIdGenerator.h"

using mmbridge::application::model::EndpointId;

namespace mmbridge::tests::driven_adapters::persistence::in_memory {

InMemoryEndpointIdGenerator::InMemoryEndpointIdGenerator(EndpointId initialEndpointId)
    : mNextEndpointId(initialEndpointId)
{
}

EndpointId InMemoryEndpointIdGenerator::next()
{
    return mNextEndpointId++;
}

}
