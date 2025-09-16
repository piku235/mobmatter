#include "ZclDeviceEndpoint.h"

#include <app/util/attribute-storage-detail.h>
#include <app/util/attribute-storage-null-handling.h>
#include <app/util/attribute-storage.h>
#include <lib/support/logging/CHIPLogging.h>

using namespace chip;

namespace {

uint16_t dynEndpointIndex = 0;
DataVersion* gDataVersionStorage[CHIP_DEVICE_CONFIG_DYNAMIC_ENDPOINT_COUNT];

uint16_t nextDynamicEndpointIndex()
{
    return dynEndpointIndex++;
}

void releaseDynamicEndpointIndex(uint16_t index)
{
    // todo: release dynamic endpoint index
}

}

namespace mobmatter::driven_adapters::matter::zcl {

void addDeviceEndpoint(EndpointId id, const EmberAfEndpointType* ep, Span<const EmberAfDeviceType> deviceTypeList, EndpointId parentEndpointId)
{
    uint16_t index = nextDynamicEndpointIndex();
    gDataVersionStorage[index] = new DataVersion[ep->clusterCount];

    CHIP_ERROR err = emberAfSetDynamicEndpoint(index, id, ep, Span<DataVersion>(gDataVersionStorage[index], ep->clusterCount), deviceTypeList, parentEndpointId);

    if (CHIP_NO_ERROR != err) {
        delete[] gDataVersionStorage[index];
        releaseDynamicEndpointIndex(index);

        ChipLogError(Zcl, "Failed adding device at endpoint %u", id);
    }
}

void removeDeviceEndpoint(EndpointId id)
{
    uint16_t index = emberAfGetDynamicIndexFromEndpoint(id);
    if (kEmberInvalidEndpointIndex == index) {
        ChipLogError(Zcl, "Failed removing device at endpoint %u: not found", id);
        return;
    }

    emberAfClearDynamicEndpoint(index);
    delete[] gDataVersionStorage[index];
    releaseDynamicEndpointIndex(index);
}

}
