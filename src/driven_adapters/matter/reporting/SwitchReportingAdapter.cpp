#include "SwitchReportingAdapter.h"

#include <app-common/zap-generated/ids/Attributes.h>
#include <app-common/zap-generated/ids/Clusters.h>
#include <app/reporting/reporting.h>

using namespace chip::app::Clusters;
using namespace mobmatter::application::model;

namespace mobmatter::driven_adapters::matter::reporting {

void SwitchReportingAdapter::handle(const SwitchTurnedOn& event)
{
    MatterReportingAttributeChangeCallback(event.endpointId, OnOff::Id, OnOff::Attributes::OnOff::Id);
}

void SwitchReportingAdapter::handle(const SwitchTurnedOff& event)
{
    MatterReportingAttributeChangeCallback(event.endpointId, OnOff::Id, OnOff::Attributes::OnOff::Id);
}

void SwitchReportingAdapter::handle(const SwitchMarkedAsReachable& event)
{
    MatterReportingAttributeChangeCallback(event.endpointId, BridgedDeviceBasicInformation::Id, BridgedDeviceBasicInformation::Attributes::Reachable::Id);
}

void SwitchReportingAdapter::handle(const SwitchMarkedAsUnreachable& event)
{
    MatterReportingAttributeChangeCallback(event.endpointId, BridgedDeviceBasicInformation::Id, BridgedDeviceBasicInformation::Attributes::Reachable::Id);
}

void SwitchReportingAdapter::handle(const SwitchRenamed& event)
{
    MatterReportingAttributeChangeCallback(event.endpointId, BridgedDeviceBasicInformation::Id, BridgedDeviceBasicInformation::Attributes::NodeLabel::Id);
}

}
