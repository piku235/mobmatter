#include "DomainEventPublisher.h"

namespace mobmatter::common::domain {

DomainEventPublisher& DomainEventPublisher::instance()
{
    static DomainEventPublisher eventPublisher;

    return eventPublisher;
}

void DomainEventPublisher::publish(const DomainEvent& event)
{
    auto it = mSubscribers.find(event.eventName());

    if (mSubscribers.end() == it) {
        return;
    }

    auto& eventHandlers = it->second;

    for (auto& eventHandler : eventHandlers) {
        eventHandler(event);
    }
}

void DomainEventPublisher::publish(DomainEventQueue& eventQueue)
{
    while (!eventQueue.empty()) {
        auto event = eventQueue.pop();
        publish(*event);
    }
}

}
