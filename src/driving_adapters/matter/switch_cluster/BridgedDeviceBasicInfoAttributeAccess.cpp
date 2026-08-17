#include "BridgedDeviceBasicInfoAttributeAccess.h"
#include "CHIPProjectAppConfig.h"
#include "application/model/Switch.h"

#include <app-common/zap-generated/ids/Attributes.h>
#include <app-common/zap-generated/ids/Clusters.h>
#include <lib/support/CodeUtils.h>
#include <lib/support/Span.h>

#include <cstdint>
#include <string>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters::BridgedDeviceBasicInformation::Attributes;
using mobmatter::application::driven_ports::SwitchRepository;
using mobmatter::application::model::Switch;

namespace {

constexpr uint8_t kBridgedDeviceBasicInfoFeatureMap = 0u;
constexpr uint16_t kBridgedDeviceBasicInfoClusterRevision = 4u;
constexpr char kProductName[] = "Switch";

}

namespace mobmatter::driving_adapters::matter::switch_cluster {

BridgedDeviceBasicInfoAttributeAccess::BridgedDeviceBasicInfoAttributeAccess(EndpointId endpointId, SwitchRepository& switchRepository)
    : AttributeAccessInterface(Optional(endpointId), Clusters::BridgedDeviceBasicInformation::Id)
    , mSwitchRepository(switchRepository)
{
}

CHIP_ERROR BridgedDeviceBasicInfoAttributeAccess::Read(const ConcreteReadAttributePath& path, AttributeValueEncoder& encoder)
{
    auto switch_ = mSwitchRepository.find(path.mEndpointId);

    if (!switch_) {
        return CHIP_ERROR_NOT_FOUND;
    }

    switch (path.mAttributeId) {
    case VendorID::Id:
        return encoder.Encode(CHIP_DEVICE_CONFIG_DEVICE_VENDOR_ID);
    case VendorName::Id:
        return encoder.Encode(CharSpan::fromCharString(CHIP_DEVICE_CONFIG_DEVICE_VENDOR_NAME));
    case ProductName::Id:
        return encoder.Encode(CharSpan::fromCharString(kProductName));
    case Reachable::Id:
        return encoder.Encode(switch_->isReachable());
    case NodeLabel::Id:
        return encoder.Encode(CharSpan::fromCharString(switch_->name().c_str()));
    case UniqueID::Id:
        return encoder.Encode(CharSpan::fromCharString(std::to_string(switch_->mobilusDeviceId()).c_str()));
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
    auto switch_ = mSwitchRepository.find(path.mEndpointId);

    if (!switch_) {
        return CHIP_ERROR_NOT_FOUND;
    }

    switch (path.mAttributeId) {
    case NodeLabel::Id: {
        CharSpan nodeLabel;
        ReturnErrorOnFailure(decoder.Decode(nodeLabel));

        if (Switch::Result::Ok == switch_->requestRename(std::string(nodeLabel.data(), nodeLabel.size()))) {
            mSwitchRepository.save(*switch_);
        }

        return CHIP_NO_ERROR;
    }
    default:
        return CHIP_ERROR_INVALID_ARGUMENT;
    }
}

}
