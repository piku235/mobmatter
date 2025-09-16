#include "LoggingDomainEventSubscriber.h"
#include <cinttypes>

#define TAG_INFO " [md=%" PRId64 ", ep=%u]"

using namespace mobmatter::application::model::window_covering;

namespace mobmatter::driven_adapters::logging {

LoggingDomainEventSubscriber::LoggingDomainEventSubscriber(logging::Logger& logger)
    : mLogger(logger)
{
}

void LoggingDomainEventSubscriber::handle(const CoverAdded& event)
{
    mLogger.notice("Added cover" TAG_INFO, event.mobilusDeviceId, event.endpointId);
}

void LoggingDomainEventSubscriber::handle(const CoverRemoved& event)
{
    mLogger.notice("Removed cover" TAG_INFO, event.mobilusDeviceId, event.endpointId);
}

void LoggingDomainEventSubscriber::handle(const CoverLiftTargetPositionChanged& event)
{
    mLogger.notice("Changed cover lift target position to %d%%" TAG_INFO, event.position.closedPercent().value(), event.mobilusDeviceId, event.endpointId);
}

void LoggingDomainEventSubscriber::handle(const CoverLiftCurrentPositionChanged& event)
{
    mLogger.notice("Changed cover lift current position to %d%%" TAG_INFO, event.position.closedPercent().value(), event.mobilusDeviceId, event.endpointId);
}

void LoggingDomainEventSubscriber::handle(const CoverMarkedAsReachable& event)
{
    mLogger.notice("Marked cover as reachable" TAG_INFO, event.mobilusDeviceId, event.endpointId);
}

void LoggingDomainEventSubscriber::handle(const CoverMarkedAsUnreachable& event)
{
    mLogger.notice("Marked cover as unreachable" TAG_INFO, event.mobilusDeviceId, event.endpointId);
}

}
