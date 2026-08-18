#include "ZclSwitchEndpointService.h"
#include "ZclDeviceEndpoint.h"

#include <app-common/zap-generated/ids/Attributes.h>
#include <app-common/zap-generated/ids/Clusters.h>
#include <app-common/zap-generated/ids/Commands.h>
#include <app/util/af-types.h>
#include <app/util/attribute-storage.h>

#define DEVICE_TYPE_BRIDGED_NODE 0x0013
#define DEVICE_TYPE_ON_OFF_SWITCH 0x0103
#define DEVICE_VERSION_DEFAULT 1

#define CHAR_STRING_SIZE 32
#define ARRAY_SIZE 64 // ATTRIBUTE_LARGEST defined by ZAP

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::DeviceLayer;
using namespace mobmatter::driven_adapters::matter::zcl;
using namespace mobmatter::application::model;

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

DECLARE_DYNAMIC_ATTRIBUTE_LIST_BEGIN(onOffAttributes)
DECLARE_DYNAMIC_ATTRIBUTE(OnOff::Attributes::OnOff::Id, BOOLEAN, 1, 0),
    DECLARE_DYNAMIC_ATTRIBUTE(OnOff::Attributes::FeatureMap::Id, BITMAP32, 4, 0),
    DECLARE_DYNAMIC_ATTRIBUTE_LIST_END(); /* ClusterRevision */

constexpr CommandId identifyCommands[] = {
    Identify::Commands::Identify::Id,
    kInvalidCommandId,
};

constexpr CommandId onOffIncomingCommands[] = {
    OnOff::Commands::Off::Id,
    OnOff::Commands::On::Id,
    OnOff::Commands::Toggle::Id,
    kInvalidCommandId,
};

DECLARE_DYNAMIC_CLUSTER_LIST_BEGIN(switchClusters)
DECLARE_DYNAMIC_CLUSTER(Identify::Id, identifyAttributes, ZAP_CLUSTER_MASK(SERVER), identifyCommands, nullptr),
    DECLARE_DYNAMIC_CLUSTER(Descriptor::Id, descriptorAttributes, ZAP_CLUSTER_MASK(SERVER), nullptr, nullptr),
    DECLARE_DYNAMIC_CLUSTER(BridgedDeviceBasicInformation::Id, bridgedDeviceBasicInfoAttributes, ZAP_CLUSTER_MASK(SERVER), nullptr, nullptr),
    DECLARE_DYNAMIC_CLUSTER(OnOff::Id, onOffAttributes, ZAP_CLUSTER_MASK(SERVER), onOffIncomingCommands, nullptr),
    DECLARE_DYNAMIC_CLUSTER_LIST_END;

DECLARE_DYNAMIC_ENDPOINT(switchEndpoint, switchClusters);

constexpr EmberAfDeviceType switchDeviceType[] = {
    { DEVICE_TYPE_ON_OFF_SWITCH, DEVICE_VERSION_DEFAULT },
    { DEVICE_TYPE_BRIDGED_NODE, DEVICE_VERSION_DEFAULT },
};

}

namespace mobmatter::driven_adapters::matter::zcl {

ZclSwitchEndpointService::ZclSwitchEndpointService(EndpointId aggregatorEndpointId)
    : mAggregatorEndpointId(aggregatorEndpointId)
{
}

void ZclSwitchEndpointService::addEndpoint(EndpointId endpointId)
{
    addDeviceEndpoint(endpointId, &switchEndpoint, Span(switchDeviceType), mAggregatorEndpointId);
}

void ZclSwitchEndpointService::removeEndpoint(EndpointId endpointId)
{
    removeDeviceEndpoint(endpointId);
}

}
