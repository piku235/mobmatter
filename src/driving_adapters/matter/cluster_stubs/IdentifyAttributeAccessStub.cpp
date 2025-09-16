#include "IdentifyAttributeAccessStub.h"

#include <app-common/zap-generated/ids/Attributes.h>
#include <app-common/zap-generated/ids/Clusters.h>

#include <cstdint>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::Identify::Attributes;

namespace {

constexpr uint8_t kIdentifyTime = 0u;
constexpr uint8_t kIdentifyType = 0u;
constexpr uint8_t kIdentifyFeatureMap = 0u;
constexpr uint8_t kIdentifyClusterRevision = 5u;

}

namespace mobmatter::driving_adapters::matter::cluster_stubs {

IdentifyAttributeAccessStub::IdentifyAttributeAccessStub()
    : AttributeAccessInterface(Optional<EndpointId>::Missing(), Identify::Id)
{
}

CHIP_ERROR IdentifyAttributeAccessStub::Read(const ConcreteReadAttributePath& path, AttributeValueEncoder& encoder)
{
    switch (path.mAttributeId) {
    case IdentifyTime::Id:
        return encoder.Encode(kIdentifyTime);
    case IdentifyType::Id:
        return encoder.Encode(kIdentifyType);
    case FeatureMap::Id:
        return encoder.Encode(kIdentifyFeatureMap);
    case ClusterRevision::Id:
        return encoder.Encode(kIdentifyClusterRevision);
    default:
        return CHIP_ERROR_INVALID_ARGUMENT;
    }
}

}
