#pragma once

#include "DomainEvent.h"
#include "DomainEventQueue.h"

#include <memory>

namespace mobmatter::common::domain {

class Entity {
protected:
    static void raise(std::unique_ptr<DomainEvent> event) { DomainEventQueue::instance().push(std::move(event)); }
};

}
