#include "DomainEventPublisher.h"

namespace mmbridge::common::domain {

DomainEventPublisher& DomainEventPublisher::instance()
{
    static DomainEventPublisher instance;

    return instance;
}

void DomainEventPublisher::defer(std::unique_ptr<DomainEvent> event)
{
    mDeferredEvents.push(std::move(event));
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

void DomainEventPublisher::publishDeferred()
{
    for (; !mDeferredEvents.empty(); mDeferredEvents.pop()) {
        publish(*mDeferredEvents.front());
    }
}

}
