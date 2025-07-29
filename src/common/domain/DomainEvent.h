#pragma once

namespace mmbridge::common::domain {

struct DomainEvent {
    static constexpr char kEventName[] = ""; // override by shadowing it

    virtual ~DomainEvent() = default;
    virtual const char* eventName() const = 0;
};

}
