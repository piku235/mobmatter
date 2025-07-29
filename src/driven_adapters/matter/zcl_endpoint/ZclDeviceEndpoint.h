#pragma once

#include <app/util/af-types.h>

namespace mmbridge::driven_adapters::matter::zcl_endpoint {

void addDeviceEndpoint(chip::EndpointId id, const EmberAfEndpointType* ep, chip::Span<const EmberAfDeviceType> deviceTypeList, chip::EndpointId parentEndpointId);
void removeDeviceEndpoint(chip::EndpointId id);

}
