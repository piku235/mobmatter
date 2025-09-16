#pragma once

#include "DomainEventSubscriber.h"

namespace mobmatter::common::domain {

template <class... TEvents>
class MultiDomainEventSubscriber : public DomainEventSubscriber<TEvents>... {
public:
    virtual ~MultiDomainEventSubscriber() = default;
};

}
