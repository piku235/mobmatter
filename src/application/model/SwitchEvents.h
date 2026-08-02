#pragma once

#include "application/model/EndpointId.h"
#include "application/model/MobilusDeviceId.h"
#include "common/domain/DomainEvent.h"

#include <string>
#include <utility>

namespace mobmatter::application::model {

struct SwitchAdded : common::domain::DomainEvent {
    static constexpr char kEventName[] = "switch_added";

    const EndpointId endpointId;
    const MobilusDeviceId mobilusDeviceId;

    SwitchAdded(EndpointId aEndpointId, MobilusDeviceId aMobilusDeviceId)
        : endpointId(aEndpointId)
        , mobilusDeviceId(aMobilusDeviceId)
    {
    }

    const char* eventName() const override { return kEventName; }
};

struct SwitchRemoved : common::domain::DomainEvent {
    static constexpr char kEventName[] = "switch_removed";

    const EndpointId endpointId;
    const MobilusDeviceId mobilusDeviceId;

    SwitchRemoved(EndpointId aEndpointId, MobilusDeviceId aMobilusDeviceId)
        : endpointId(aEndpointId)
        , mobilusDeviceId(aMobilusDeviceId)
    {
    }

    const char* eventName() const override { return kEventName; }
};

struct SwitchTurnedOn : common::domain::DomainEvent {
    static constexpr char kEventName[] = "switch_turned_on";

    const EndpointId endpointId;
    const MobilusDeviceId mobilusDeviceId;

    SwitchTurnedOn(EndpointId aEndpointId, MobilusDeviceId aMobilusDeviceId)
        : endpointId(aEndpointId)
        , mobilusDeviceId(aMobilusDeviceId)
    {
    }

    const char* eventName() const override { return kEventName; }
};

struct SwitchTurnedOff : common::domain::DomainEvent {
    static constexpr char kEventName[] = "switch_turned_off";

    const EndpointId endpointId;
    const MobilusDeviceId mobilusDeviceId;

    SwitchTurnedOff(EndpointId aEndpointId, MobilusDeviceId aMobilusDeviceId)
        : endpointId(aEndpointId)
        , mobilusDeviceId(aMobilusDeviceId)
    {
    }

    const char* eventName() const override { return kEventName; }
};

struct SwitchTurnOnRequested : common::domain::DomainEvent {
    static constexpr char kEventName[] = "switch_turn_on_requested";

    const EndpointId endpointId;
    const MobilusDeviceId mobilusDeviceId;

    SwitchTurnOnRequested(EndpointId aEndpointId, MobilusDeviceId aMobilusDeviceId)
        : endpointId(aEndpointId)
        , mobilusDeviceId(aMobilusDeviceId)
    {
    }

    const char* eventName() const override { return kEventName; }
};

struct SwitchTurnOffRequested : common::domain::DomainEvent {
    static constexpr char kEventName[] = "switch_turn_off_requested";

    const EndpointId endpointId;
    const MobilusDeviceId mobilusDeviceId;

    SwitchTurnOffRequested(EndpointId aEndpointId, MobilusDeviceId aMobilusDeviceId)
        : endpointId(aEndpointId)
        , mobilusDeviceId(aMobilusDeviceId)
    {
    }

    const char* eventName() const override { return kEventName; }
};

struct SwitchMarkedAsReachable : common::domain::DomainEvent {
    static constexpr char kEventName[] = "switch_marked_as_reachable";

    const EndpointId endpointId;
    const MobilusDeviceId mobilusDeviceId;

    SwitchMarkedAsReachable(EndpointId aEndpointId, MobilusDeviceId aMobilusDeviceId)
        : endpointId(aEndpointId)
        , mobilusDeviceId(aMobilusDeviceId)
    {
    }

    const char* eventName() const override { return kEventName; }
};

struct SwitchMarkedAsUnreachable : common::domain::DomainEvent {
    static constexpr char kEventName[] = "switch_marked_as_unreachable";

    const EndpointId endpointId;
    const MobilusDeviceId mobilusDeviceId;

    SwitchMarkedAsUnreachable(EndpointId aEndpointId, MobilusDeviceId aMobilusDeviceId)
        : endpointId(aEndpointId)
        , mobilusDeviceId(aMobilusDeviceId)
    {
    }

    const char* eventName() const override { return kEventName; }
};

struct SwitchNameChanged : common::domain::DomainEvent {
    static constexpr char kEventName[] = "switch_name_changed";

    const EndpointId endpointId;
    const MobilusDeviceId mobilusDeviceId;
    const std::string name;

    SwitchNameChanged(EndpointId aEndpointId, MobilusDeviceId aMobilusDeviceId, std::string aName)
        : endpointId(aEndpointId)
        , mobilusDeviceId(aMobilusDeviceId)
        , name(std::move(aName))
    {
    }

    const char* eventName() const override { return kEventName; }
};

struct SwitchRenameRequested : common::domain::DomainEvent {
    static constexpr char kEventName[] = "switch_rename_requested";

    const EndpointId endpointId;
    const MobilusDeviceId mobilusDeviceId;
    const std::string name;

    SwitchRenameRequested(EndpointId aEndpointId, MobilusDeviceId aMobilusDeviceId, std::string aName)
        : endpointId(aEndpointId)
        , mobilusDeviceId(aMobilusDeviceId)
        , name(std::move(aName))
    {
    }

    const char* eventName() const override { return kEventName; }
};

}
