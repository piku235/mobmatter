#pragma once

namespace mobmatter::common::domain {

struct DomainEvent {
    virtual ~DomainEvent() = default;
    virtual const char* eventName() const = 0;
};

}
