#include "BridgedDeviceBasicInfoAttributeAccess.h"
#include "CHIPProjectAppConfig.h"

#include <app-common/zap-generated/ids/Attributes.h>
#include <app-common/zap-generated/ids/Clusters.h>
#include <lib/support/Span.h>

#include <cstdint>
#include <string>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters::BridgedDeviceBasicInformation::Attributes;

namespace {

constexpr uint8_t kBridgedDeviceBasicInfoFeatureMap = 0u;
constexpr uint16_t kBridgedDeviceBasicInfoClusterRevision = 4u;

}

namespace mobmatter::driving_adapters::matter::bridged_device_cluster {

BridgedDeviceBasicInfoAttributeAccess::BridgedDeviceBasicInfoAttributeAccess(EndpointId endpointId, BridgedDeviceBasicInfoLoader& loader)
    : AttributeAccessInterface(Optional(endpointId), Clusters::BridgedDeviceBasicInformation::Id)
    , mLoader(loader)
{
}

CHIP_ERROR BridgedDeviceBasicInfoAttributeAccess::Read(const ConcreteReadAttributePath& path, AttributeValueEncoder& encoder)
{
    auto data = mLoader.load(path.mEndpointId);

    if (!data) {
        return CHIP_ERROR_NOT_FOUND;
    }

    switch (path.mAttributeId) {
    case VendorID::Id:
        return encoder.Encode(CHIP_DEVICE_CONFIG_DEVICE_VENDOR_ID);
    case VendorName::Id:
        return encoder.Encode(CharSpan::fromCharString(CHIP_DEVICE_CONFIG_DEVICE_VENDOR_NAME));
    case ProductName::Id:
        return encoder.Encode(CharSpan::fromCharString(data->productName.c_str()));
    case Reachable::Id:
        return encoder.Encode(data->reachable);
    case NodeLabel::Id:
        return encoder.Encode(CharSpan::fromCharString(data->nodeLabel.c_str()));
    case UniqueID::Id:
        return encoder.Encode(CharSpan::fromCharString(data->uniqueId.c_str()));
    case FeatureMap::Id:
        return encoder.Encode(kBridgedDeviceBasicInfoFeatureMap);
    case ClusterRevision::Id:
        return encoder.Encode(kBridgedDeviceBasicInfoClusterRevision);
    default:
        return CHIP_ERROR_INVALID_ARGUMENT;
    }
}

}
