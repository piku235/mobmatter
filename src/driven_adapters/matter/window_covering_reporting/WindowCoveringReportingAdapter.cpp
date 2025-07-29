#include "WindowCoveringReportingAdapter.h"

#include <app-common/zap-generated/ids/Attributes.h>
#include <app-common/zap-generated/ids/Clusters.h>
#include <app/reporting/reporting.h>

using namespace chip::app::Clusters;
using namespace mmbridge::application::model::window_covering;

namespace mmbridge::driven_adapters::matter::window_covering_reporting {

void WindowCoveringReportingAdapter::handle(const CoverLiftCurrentPositionChanged& event)
{
    MatterReportingAttributeChangeCallback(event.endpointId, WindowCovering::Id, WindowCovering::Attributes::CurrentPositionLiftPercent100ths::Id);
}

void WindowCoveringReportingAdapter::handle(const CoverLiftTargetPositionChanged& event)
{
    MatterReportingAttributeChangeCallback(event.endpointId, WindowCovering::Id, WindowCovering::Attributes::TargetPositionLiftPercent100ths::Id);
}

void WindowCoveringReportingAdapter::handle(const CoverMarkedAsReachable& event)
{
    MatterReportingAttributeChangeCallback(event.endpointId, BridgedDeviceBasicInformation::Id, BridgedDeviceBasicInformation::Attributes::Reachable::Id);
}

void WindowCoveringReportingAdapter::handle(const CoverMarkedAsUnreachable& event)
{
    MatterReportingAttributeChangeCallback(event.endpointId, BridgedDeviceBasicInformation::Id, BridgedDeviceBasicInformation::Attributes::Reachable::Id);
}

void WindowCoveringReportingAdapter::handle(const CoverOperationalStatusChanged& event)
{
    MatterReportingAttributeChangeCallback(event.endpointId, WindowCovering::Id, WindowCovering::Attributes::OperationalStatus::Id);
}

}
