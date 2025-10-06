#include "ZclDeviceEndpoint.h"

#include <app/util/attribute-storage-detail.h>
#include <app/util/attribute-storage-null-handling.h>
#include <app/util/attribute-storage.h>
#include <lib/support/logging/CHIPLogging.h>

using namespace chip;

namespace {

constexpr uint16_t kOutOfRangeEndpointIndex = 0xFFFF;
DataVersion* gDataVersionStorage[CHIP_DEVICE_CONFIG_DYNAMIC_ENDPOINT_COUNT];

uint16_t nextEndpointIndex()
{
    for (uint16_t index = 0; index < CHIP_DEVICE_CONFIG_DYNAMIC_ENDPOINT_COUNT; index++) {
        if (nullptr == gDataVersionStorage[index]) {
            return index;
        }
    }

    return kOutOfRangeEndpointIndex;
}

}

namespace mobmatter::driven_adapters::matter::zcl {

void addDeviceEndpoint(EndpointId id, const EmberAfEndpointType* ep, Span<const EmberAfDeviceType> deviceTypeList, EndpointId parentEndpointId)
{
    auto index = nextEndpointIndex();
    if (kOutOfRangeEndpointIndex == index) {
        ChipLogError(Zcl, "Run out of available endpoints, max endpoint count: %u", CHIP_DEVICE_CONFIG_DYNAMIC_ENDPOINT_COUNT);
        return;
    }

    auto dataVersion = new DataVersion[ep->clusterCount];
    CHIP_ERROR err = emberAfSetDynamicEndpoint(index, id, ep, Span<DataVersion>(dataVersion, ep->clusterCount), deviceTypeList, parentEndpointId);

    if (CHIP_NO_ERROR != err) {
        delete[] dataVersion;
        ChipLogError(Zcl, "Failed adding device at endpoint %u", id);

        return;
    }

    gDataVersionStorage[index] = dataVersion;
}

void removeDeviceEndpoint(EndpointId id)
{
    auto index = emberAfGetDynamicIndexFromEndpoint(id);
    if (kEmberInvalidEndpointIndex == index) {
        ChipLogError(Zcl, "Failed removing device at endpoint %u: not found", id);
        return;
    }

    emberAfClearDynamicEndpoint(index);
    delete[] gDataVersionStorage[index];
    gDataVersionStorage[index] = nullptr;
}

}
