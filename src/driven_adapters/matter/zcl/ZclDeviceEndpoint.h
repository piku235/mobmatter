#pragma once

#include <app/util/af-types.h>

namespace mobmatter::driven_adapters::matter::zcl {

void addDeviceEndpoint(chip::EndpointId id, const EmberAfEndpointType* ep, chip::Span<const EmberAfDeviceType> deviceTypeList, chip::EndpointId parentEndpointId);
void removeDeviceEndpoint(chip::EndpointId id);

}
