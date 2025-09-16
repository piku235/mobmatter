#include "DomainEventQueue.h"

namespace mobmatter::common::domain {

DomainEventQueue& DomainEventQueue::instance()
{
    static DomainEventQueue eventQueue;

    return eventQueue;
}

void DomainEventQueue::push(std::unique_ptr<DomainEvent> event)
{
    mEvents.push(std::move(event));
}

std::unique_ptr<DomainEvent> DomainEventQueue::pop()
{
    if (empty()) {
        return nullptr;
    }

    auto event = std::move(mEvents.front());
    mEvents.pop();

    return event;
}

const DomainEvent* DomainEventQueue::peek() const
{
    if (empty()) {
        return nullptr;
    }

    return mEvents.front().get();
}

void DomainEventQueue::clear()
{
    QueuedEvents emptyEventQueue;
    mEvents.swap(emptyEventQueue);
}

size_t DomainEventQueue::size() const
{
    return mEvents.size();
}

bool DomainEventQueue::empty() const
{
    return mEvents.empty();
}

}
