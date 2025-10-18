#include "BridgedDeviceBasicInfoAttributeAccess.h"
#include "CHIPProjectAppConfig.h"
#include "application/model/window_covering/Cover.h"

#include <app-common/zap-generated/ids/Attributes.h>
#include <app-common/zap-generated/ids/Clusters.h>
#include <lib/support/CodeUtils.h>
#include <lib/support/Span.h>

#include <cstdint>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::BridgedDeviceBasicInformation::Attributes;
using mobmatter::application::driven_ports::CoverRepository;
using mobmatter::application::model::window_covering::Cover;

namespace {

constexpr uint8_t kBridgedDeviceBasicInfoFeatureMap = 0u;
constexpr uint16_t kBridgedDeviceBasicInfoClusterRevision = 4u;

}

namespace mobmatter::driving_adapters::matter::cover_cluster {

BridgedDeviceBasicInfoAttributeAccess::BridgedDeviceBasicInfoAttributeAccess(CoverRepository& coverRepository)
    : AttributeAccessInterface(Optional<EndpointId>::Missing(), BridgedDeviceBasicInformation::Id)
    , mCoverRepository(coverRepository)
{
}

CHIP_ERROR BridgedDeviceBasicInfoAttributeAccess::Read(const ConcreteReadAttributePath& path, AttributeValueEncoder& encoder)
{
    auto cover = mCoverRepository.find(path.mEndpointId);

    if (!cover) {
        return CHIP_ERROR_NOT_FOUND;
    }

    switch (path.mAttributeId) {
    case VendorID::Id:
        return encoder.Encode(CHIP_DEVICE_CONFIG_DEVICE_VENDOR_ID);
    case VendorName::Id:
        return encoder.Encode(CharSpan::fromCharString(CHIP_DEVICE_CONFIG_DEVICE_VENDOR_NAME));
    case ProductName::Id:
        return encoder.Encode(CharSpan::fromCharString(cover->specification().model().c_str()));
    case Reachable::Id:
        return encoder.Encode(cover->isReachable());
    case NodeLabel::Id:
        return encoder.Encode(CharSpan::fromCharString(cover->name().c_str()));
    case UniqueID::Id:
        return encoder.Encode(CharSpan::fromCharString(cover->uniqueId().value().c_str()));
    case FeatureMap::Id:
        return encoder.Encode(kBridgedDeviceBasicInfoFeatureMap);
    case ClusterRevision::Id:
        return encoder.Encode(kBridgedDeviceBasicInfoClusterRevision);
    default:
        return CHIP_ERROR_INVALID_ARGUMENT;
    }
}

CHIP_ERROR BridgedDeviceBasicInfoAttributeAccess::Write(const ConcreteDataAttributePath& path, AttributeValueDecoder& decoder)
{
    auto cover = mCoverRepository.find(path.mEndpointId);

    if (!cover) {
        return CHIP_ERROR_NOT_FOUND;
    }

    switch (path.mAttributeId) {
    case NodeLabel::Id: {
        CharSpan nodeLabel;
        ReturnErrorOnFailure(decoder.Decode(nodeLabel));

        if (Cover::Result::Ok == cover->requestRename(std::string(nodeLabel.data(), nodeLabel.size()))) {
            mCoverRepository.save(*cover);
        }

        return CHIP_NO_ERROR;
    }
    default:
        return CHIP_ERROR_INVALID_ARGUMENT;
    }
}

}
