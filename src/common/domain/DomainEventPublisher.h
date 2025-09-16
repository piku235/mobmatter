#pragma once

#include "DomainEvent.h"
#include "DomainEventQueue.h"
#include "DomainEventSubscriber.h"
#include "MultiDomainEventSubscriber.h"

#include <functional>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace mobmatter::common::domain {

class DomainEventPublisher final {
public:
    static DomainEventPublisher& instance();

    DomainEventPublisher(DomainEventPublisher&& other) = delete;
    DomainEventPublisher& operator=(DomainEventPublisher&& other) = delete;

    DomainEventPublisher(const DomainEventPublisher& other) = delete;
    DomainEventPublisher& operator=(const DomainEventPublisher& other) = delete;

    void publish(const DomainEvent& event);
    void publish(DomainEventQueue& eventQueue);

    template <class TEvent, class = std::enable_if_t<std::is_base_of_v<DomainEvent, TEvent>>>
    void subscribe(DomainEventSubscriber<TEvent>& subscriber)
    {
        auto& eventHandlers = mSubscribers[TEvent::kEventName];

        eventHandlers.push_back([&](auto& event) {
            subscriber.handle(static_cast<const TEvent&>(event));
        });
    }

    template <class... TEvents>
    void subscribe(MultiDomainEventSubscriber<TEvents...>& subscriber)
    {
        (subscribe(static_cast<DomainEventSubscriber<TEvents>&>(subscriber)), ...);
    }

private:
    using EventHandler = std::function<void(const DomainEvent&)>;

    std::unordered_map<std::string, std::vector<EventHandler>> mSubscribers;

    DomainEventPublisher() = default;
};

}
