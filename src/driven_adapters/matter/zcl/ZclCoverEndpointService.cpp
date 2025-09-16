#include "ZclCoverEndpointService.h"
#include "ZclDeviceEndpoint.h"

#include <app-common/zap-generated/ids/Attributes.h>
#include <app-common/zap-generated/ids/Clusters.h>
#include <app-common/zap-generated/ids/Commands.h>
#include <app/util/af-types.h>
#include <app/util/attribute-storage.h>

#define DEVICE_TYPE_BRIDGED_NODE 0x0013
#define DEVICE_TYPE_WINDOW_COVERING 0x0202
#define DEVICE_VERSION_DEFAULT 1

#define CHAR_STRING_SIZE 32
#define PERCENT100THS_SIZE 2
#define ARRAY_SIZE 64 // ATTRIBUTE_LARGEST defined by ZAP

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::DeviceLayer;
using namespace mobmatter::driven_adapters::matter::zcl;
using namespace mobmatter::application::model::window_covering;

namespace {

DECLARE_DYNAMIC_ATTRIBUTE_LIST_BEGIN(identifyAttributes)
DECLARE_DYNAMIC_ATTRIBUTE(Identify::Attributes::IdentifyTime::Id, INT16U, 2, 0),
    DECLARE_DYNAMIC_ATTRIBUTE(Identify::Attributes::IdentifyType::Id, ENUM8, 1, 0),
    DECLARE_DYNAMIC_ATTRIBUTE_LIST_END(); /* ClusterRevision */

DECLARE_DYNAMIC_ATTRIBUTE_LIST_BEGIN(descriptorAttributes)
DECLARE_DYNAMIC_ATTRIBUTE(Descriptor::Attributes::DeviceTypeList::Id, ARRAY, ARRAY_SIZE, 0),
    DECLARE_DYNAMIC_ATTRIBUTE(Descriptor::Attributes::ServerList::Id, ARRAY, ARRAY_SIZE, 0),
    DECLARE_DYNAMIC_ATTRIBUTE(Descriptor::Attributes::ClientList::Id, ARRAY, ARRAY_SIZE, 0),
    DECLARE_DYNAMIC_ATTRIBUTE(Descriptor::Attributes::PartsList::Id, ARRAY, ARRAY_SIZE, 0),
    DECLARE_DYNAMIC_ATTRIBUTE(Descriptor::Attributes::FeatureMap::Id, BITMAP32, 4, 0),
    DECLARE_DYNAMIC_ATTRIBUTE_LIST_END(); /* ClusterRevision */

DECLARE_DYNAMIC_ATTRIBUTE_LIST_BEGIN(bridgedDeviceBasicInfoAttributes)
DECLARE_DYNAMIC_ATTRIBUTE(BridgedDeviceBasicInformation::Attributes::VendorID::Id, VENDOR_ID, 2, 0),
    DECLARE_DYNAMIC_ATTRIBUTE(BridgedDeviceBasicInformation::Attributes::VendorName::Id, CHAR_STRING, CHAR_STRING_SIZE, 0),
    DECLARE_DYNAMIC_ATTRIBUTE(BridgedDeviceBasicInformation::Attributes::ProductName::Id, CHAR_STRING, CHAR_STRING_SIZE, 0),
    DECLARE_DYNAMIC_ATTRIBUTE(BridgedDeviceBasicInformation::Attributes::NodeLabel::Id, CHAR_STRING, CHAR_STRING_SIZE, 0),
    DECLARE_DYNAMIC_ATTRIBUTE(BridgedDeviceBasicInformation::Attributes::Reachable::Id, BOOLEAN, 1, 0),
    DECLARE_DYNAMIC_ATTRIBUTE(BridgedDeviceBasicInformation::Attributes::UniqueID::Id, CHAR_STRING, CHAR_STRING_SIZE, 0),
    DECLARE_DYNAMIC_ATTRIBUTE(BridgedDeviceBasicInformation::Attributes::FeatureMap::Id, BITMAP32, 4, 0),
    DECLARE_DYNAMIC_ATTRIBUTE_LIST_END(); /* ClusterRevision */

DECLARE_DYNAMIC_ATTRIBUTE_LIST_BEGIN(coverAttributes)
DECLARE_DYNAMIC_ATTRIBUTE(WindowCovering::Attributes::Type::Id, ENUM8, 1, 0),
    DECLARE_DYNAMIC_ATTRIBUTE(WindowCovering::Attributes::EndProductType::Id, ENUM8, 1, 0),
    DECLARE_DYNAMIC_ATTRIBUTE(WindowCovering::Attributes::ConfigStatus::Id, BITMAP8, 1, 0),
    DECLARE_DYNAMIC_ATTRIBUTE(WindowCovering::Attributes::OperationalStatus::Id, BITMAP8, 1, 0),
    DECLARE_DYNAMIC_ATTRIBUTE(WindowCovering::Attributes::CurrentPositionLiftPercent100ths::Id, PERCENT100THS, PERCENT100THS_SIZE, 0),
    DECLARE_DYNAMIC_ATTRIBUTE(WindowCovering::Attributes::TargetPositionLiftPercent100ths::Id, PERCENT100THS, PERCENT100THS_SIZE, 0),
    DECLARE_DYNAMIC_ATTRIBUTE(WindowCovering::Attributes::Mode::Id, BITMAP8, 1, 0),
    DECLARE_DYNAMIC_ATTRIBUTE(WindowCovering::Attributes::FeatureMap::Id, BITMAP32, 4, 0),
    DECLARE_DYNAMIC_ATTRIBUTE_LIST_END(); /* ClusterRevision */

constexpr CommandId identifyCommands[] = {
    Identify::Commands::Identify::Id,
    kInvalidCommandId,
};

constexpr CommandId coverPositionAwareLiftCommands[] = {
    WindowCovering::Commands::UpOrOpen::Id,
    WindowCovering::Commands::DownOrClose::Id,
    WindowCovering::Commands::StopMotion::Id,
    WindowCovering::Commands::GoToLiftPercentage::Id,
    kInvalidCommandId,
};

DECLARE_DYNAMIC_CLUSTER_LIST_BEGIN(coverPositionAwareLiftClusters)
DECLARE_DYNAMIC_CLUSTER(Identify::Id, identifyAttributes, ZAP_CLUSTER_MASK(SERVER), identifyCommands, nullptr),
    DECLARE_DYNAMIC_CLUSTER(Descriptor::Id, descriptorAttributes, ZAP_CLUSTER_MASK(SERVER), nullptr, nullptr),
    DECLARE_DYNAMIC_CLUSTER(BridgedDeviceBasicInformation::Id, bridgedDeviceBasicInfoAttributes, ZAP_CLUSTER_MASK(SERVER), nullptr, nullptr),
    DECLARE_DYNAMIC_CLUSTER(WindowCovering::Id, coverAttributes, ZAP_CLUSTER_MASK(SERVER), coverPositionAwareLiftCommands, nullptr),
    DECLARE_DYNAMIC_CLUSTER_LIST_END;

DECLARE_DYNAMIC_ENDPOINT(coverPositionAwareLiftEndpoint, coverPositionAwareLiftClusters);

constexpr CommandId coverEdgePositionAwareLiftCommands[] = {
    WindowCovering::Commands::UpOrOpen::Id,
    WindowCovering::Commands::DownOrClose::Id,
    WindowCovering::Commands::StopMotion::Id,
    kInvalidCommandId,
};

DECLARE_DYNAMIC_CLUSTER_LIST_BEGIN(coverEdgePositionAwareLiftClusters)
DECLARE_DYNAMIC_CLUSTER(Identify::Id, identifyAttributes, ZAP_CLUSTER_MASK(SERVER), identifyCommands, nullptr),
    DECLARE_DYNAMIC_CLUSTER(Descriptor::Id, descriptorAttributes, ZAP_CLUSTER_MASK(SERVER), nullptr, nullptr),
    DECLARE_DYNAMIC_CLUSTER(BridgedDeviceBasicInformation::Id, bridgedDeviceBasicInfoAttributes, ZAP_CLUSTER_MASK(SERVER), nullptr, nullptr),
    DECLARE_DYNAMIC_CLUSTER(WindowCovering::Id, coverAttributes, ZAP_CLUSTER_MASK(SERVER), coverEdgePositionAwareLiftCommands, nullptr),
    DECLARE_DYNAMIC_CLUSTER_LIST_END;

DECLARE_DYNAMIC_ENDPOINT(coverEdgePositionAwareLiftEndpoint, coverEdgePositionAwareLiftClusters);

const EmberAfDeviceType coverDeviceTypes[] = {
    { DEVICE_TYPE_WINDOW_COVERING, DEVICE_VERSION_DEFAULT },
    { DEVICE_TYPE_BRIDGED_NODE, DEVICE_VERSION_DEFAULT },
};

}

namespace mobmatter::driven_adapters::matter::zcl {

ZclCoverEndpointService::ZclCoverEndpointService(EndpointId aggregatorEndpointId)
    : mAggregatorEndpointId(aggregatorEndpointId)
{
}

void ZclCoverEndpointService::addEndpoint(EndpointId endpointId, const wc::CoverSpecification& specification)
{
    EmberAfEndpointType* endpointType = &coverPositionAwareLiftEndpoint;

    if (specification.featureFlags().has(CoverFeature::EdgePositionAwareLift)) {
        endpointType = &coverEdgePositionAwareLiftEndpoint;
    }

    addDeviceEndpoint(endpointId, endpointType, Span<const EmberAfDeviceType>(coverDeviceTypes), mAggregatorEndpointId);
}

void ZclCoverEndpointService::removeEndpoint(EndpointId endpointId)
{
    removeDeviceEndpoint(endpointId);
}

}
