#pragma once

namespace mobmatter::common::domain {

template <class TEvent>
class DomainEventSubscriber {
public:
    virtual ~DomainEventSubscriber() = default;
    virtual void handle(const TEvent& event) = 0;
};

}
