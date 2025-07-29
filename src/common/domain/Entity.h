#pragma once

#include "DomainEvent.h"
#include "DomainEventPublisher.h"

#include <memory>

namespace mmbridge::common::domain {

class Entity {
protected:
    static void raise(std::unique_ptr<DomainEvent> event) { DomainEventPublisher::instance().defer(std::move(event)); }
};

}
