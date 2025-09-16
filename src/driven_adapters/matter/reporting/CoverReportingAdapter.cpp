#include "CoverReportingAdapter.h"

#include <app-common/zap-generated/ids/Attributes.h>
#include <app-common/zap-generated/ids/Clusters.h>
#include <app/reporting/reporting.h>

using namespace chip::app::Clusters;
using namespace mobmatter::application::model::window_covering;

namespace mobmatter::driven_adapters::matter::reporting {

void CoverReportingAdapter::handle(const CoverLiftCurrentPositionChanged& event)
{
    MatterReportingAttributeChangeCallback(event.endpointId, WindowCovering::Id, WindowCovering::Attributes::CurrentPositionLiftPercent100ths::Id);
}

void CoverReportingAdapter::handle(const CoverLiftTargetPositionChanged& event)
{
    MatterReportingAttributeChangeCallback(event.endpointId, WindowCovering::Id, WindowCovering::Attributes::TargetPositionLiftPercent100ths::Id);
}

void CoverReportingAdapter::handle(const CoverMarkedAsReachable& event)
{
    MatterReportingAttributeChangeCallback(event.endpointId, BridgedDeviceBasicInformation::Id, BridgedDeviceBasicInformation::Attributes::Reachable::Id);
}

void CoverReportingAdapter::handle(const CoverMarkedAsUnreachable& event)
{
    MatterReportingAttributeChangeCallback(event.endpointId, BridgedDeviceBasicInformation::Id, BridgedDeviceBasicInformation::Attributes::Reachable::Id);
}

void CoverReportingAdapter::handle(const CoverOperationalStatusChanged& event)
{
    MatterReportingAttributeChangeCallback(event.endpointId, WindowCovering::Id, WindowCovering::Attributes::OperationalStatus::Id);
}

}
