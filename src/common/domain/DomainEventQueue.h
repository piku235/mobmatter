#pragma once

#include "DomainEvent.h"

#include <memory>
#include <queue>

namespace mobmatter::common::domain {

class DomainEventQueue final {
public:
    static DomainEventQueue& instance();

    DomainEventQueue(DomainEventQueue&& other) = delete;
    DomainEventQueue& operator=(DomainEventQueue&& other) = delete;

    DomainEventQueue(const DomainEventQueue& other) = delete;
    DomainEventQueue& operator=(const DomainEventQueue& other) = delete;

    void push(std::unique_ptr<DomainEvent> event);
    std::unique_ptr<DomainEvent> pop();
    const DomainEvent* peek() const;
    void clear();
    size_t size() const;
    bool empty() const;

private:
    using QueuedEvents = std::queue<std::unique_ptr<DomainEvent>>;

    QueuedEvents mEvents;

    DomainEventQueue() = default;
};

}
