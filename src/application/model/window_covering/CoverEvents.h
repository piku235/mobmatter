#pragma once

#include "CoverMotion.h"
#include "CoverSpecification.h"
#include "Position.h"
#include "application/model/EndpointId.h"
#include "application/model/MobilusDeviceId.h"
#include "common/domain/DomainEvent.h"

#include <string>

namespace mobmatter::application::model::window_covering {

struct CoverOpenRequested : common::domain::DomainEvent {
    static constexpr char kEventName[] = "cover_open_requested";

    const EndpointId endpointId;
    const MobilusDeviceId mobilusDeviceId;

    CoverOpenRequested(EndpointId aEndpointId, MobilusDeviceId aMobilusDeviceId)
        : endpointId(aEndpointId)
        , mobilusDeviceId(aMobilusDeviceId)
    {
    }

    const char* eventName() const override { return kEventName; }
};

struct CoverCloseRequested : common::domain::DomainEvent {
    static constexpr char kEventName[] = "cover_close_requested";

    const EndpointId endpointId;
    const MobilusDeviceId mobilusDeviceId;

    CoverCloseRequested(EndpointId aEndpointId, MobilusDeviceId aMobilusDeviceId)
        : endpointId(aEndpointId)
        , mobilusDeviceId(aMobilusDeviceId)
    {
    }

    const char* eventName() const override { return kEventName; }
};

struct CoverAdded : common::domain::DomainEvent {
    static constexpr char kEventName[] = "cover_added";

    const EndpointId endpointId;
    const MobilusDeviceId mobilusDeviceId;
    const CoverSpecification specification;

    CoverAdded(EndpointId aEndpointId, MobilusDeviceId aMobilusDeviceId, CoverSpecification aSpecification)
        : endpointId(aEndpointId)
        , mobilusDeviceId(aMobilusDeviceId)
        , specification(std::move(aSpecification))
    {
    }

    const char* eventName() const override { return kEventName; }
};

struct CoverRemoved : common::domain::DomainEvent {
    static constexpr char kEventName[] = "cover_removed";

    const EndpointId endpointId;
    const MobilusDeviceId mobilusDeviceId;

    CoverRemoved(EndpointId aEndpointId, MobilusDeviceId aMobilusDeviceId)
        : endpointId(aEndpointId)
        , mobilusDeviceId(aMobilusDeviceId)
    {
    }

    const char* eventName() const override { return kEventName; }
};

struct CoverLiftCurrentPositionChanged : common::domain::DomainEvent {
    static constexpr char kEventName[] = "cover_lift_current_position_changed";

    const EndpointId endpointId;
    const MobilusDeviceId mobilusDeviceId;
    const Position position;

    CoverLiftCurrentPositionChanged(EndpointId aEndpointId, MobilusDeviceId aMobilusDeviceId, Position aPosition)
        : endpointId(aEndpointId)
        , mobilusDeviceId(aMobilusDeviceId)
        , position(aPosition)
    {
    }

    const char* eventName() const override { return kEventName; }
};

struct CoverLiftMotionChanged : common::domain::DomainEvent {
    static constexpr char kEventName[] = "cover_lift_motion_changed";

    const EndpointId endpointId;
    const MobilusDeviceId mobilusDeviceId;
    const CoverMotion motion;

    CoverLiftMotionChanged(EndpointId aEndpointId, MobilusDeviceId aMobilusDeviceId, CoverMotion aMotion)
        : endpointId(aEndpointId)
        , mobilusDeviceId(aMobilusDeviceId)
        , motion(aMotion)
    {
    }

    const char* eventName() const override { return kEventName; }
};

struct CoverLiftRequested : common::domain::DomainEvent {
    static constexpr char kEventName[] = "cover_lift_requested";

    const EndpointId endpointId;
    const MobilusDeviceId mobilusDeviceId;
    const Position position;

    CoverLiftRequested(EndpointId aEndpointId, MobilusDeviceId aMobilusDeviceId, Position aPosition)
        : endpointId(aEndpointId)
        , mobilusDeviceId(aMobilusDeviceId)
        , position(aPosition)
    {
    }

    const char* eventName() const override { return kEventName; }
};

struct CoverLiftTargetPositionChanged : common::domain::DomainEvent {
    static constexpr char kEventName[] = "cover_lift_target_position_changed";

    const EndpointId endpointId;
    const MobilusDeviceId mobilusDeviceId;
    const Position position;

    CoverLiftTargetPositionChanged(EndpointId aEndpointId, MobilusDeviceId aMobilusDeviceId, Position aPosition)
        : endpointId(aEndpointId)
        , mobilusDeviceId(aMobilusDeviceId)
        , position(aPosition)
    {
    }

    const char* eventName() const override { return kEventName; }
};

struct CoverMarkedAsReachable : common::domain::DomainEvent {
    static constexpr char kEventName[] = "cover_marked_as_reachable";

    const EndpointId endpointId;
    const MobilusDeviceId mobilusDeviceId;

    CoverMarkedAsReachable(EndpointId aEndpointId, MobilusDeviceId aMobilusDeviceId)
        : endpointId(aEndpointId)
        , mobilusDeviceId(aMobilusDeviceId)
    {
    }

    const char* eventName() const override { return kEventName; }
};

struct CoverMarkedAsUnreachable : common::domain::DomainEvent {
    static constexpr char kEventName[] = "cover_marked_as_unreachable";

    const EndpointId endpointId;
    const MobilusDeviceId mobilusDeviceId;

    CoverMarkedAsUnreachable(EndpointId aEndpointId, MobilusDeviceId aMobilusDeviceId)
        : endpointId(aEndpointId)
        , mobilusDeviceId(aMobilusDeviceId)
    {
    }

    const char* eventName() const override { return kEventName; }
};

struct CoverRenamed : common::domain::DomainEvent {
    static constexpr char kEventName[] = "cover_ren";

    const EndpointId endpointId;
    const MobilusDeviceId mobilusDeviceId;
    const std::string name;

    CoverRenamed(EndpointId aEndpointId, MobilusDeviceId aMobilusDeviceId, std::string aName)
        : endpointId(aEndpointId)
        , mobilusDeviceId(aMobilusDeviceId)
        , name(std::move(aName))
    {
    }

    const char* eventName() const override { return kEventName; }
};

struct CoverRenameRequested : common::domain::DomainEvent {
    static constexpr char kEventName[] = "cover_rename_requested";

    const EndpointId endpointId;
    const MobilusDeviceId mobilusDeviceId;
    const std::string name;

    CoverRenameRequested(EndpointId aEndpointId, MobilusDeviceId aMobilusDeviceId, std::string aName)
        : endpointId(aEndpointId)
        , mobilusDeviceId(aMobilusDeviceId)
        , name(std::move(aName))
    {
    }

    const char* eventName() const override { return kEventName; }
};

struct CoverStopMotionRequested : common::domain::DomainEvent {
    static constexpr char kEventName[] = "cover_stop_motion_requested";

    const EndpointId endpointId;
    const MobilusDeviceId mobilusDeviceId;

    CoverStopMotionRequested(EndpointId aEndpointId, MobilusDeviceId aMobilusDeviceId)
        : endpointId(aEndpointId)
        , mobilusDeviceId(aMobilusDeviceId)
    {
    }

    const char* eventName() const override { return kEventName; }
};

struct CoverTiltCurrentPositionChanged : common::domain::DomainEvent {
    static constexpr char kEventName[] = "cover_tilt_current_position_changed";

    const EndpointId endpointId;
    const MobilusDeviceId mobilusDeviceId;
    const Position position;

    CoverTiltCurrentPositionChanged(EndpointId aEndpointId, MobilusDeviceId aMobilusDeviceId, Position aPosition)
        : endpointId(aEndpointId)
        , mobilusDeviceId(aMobilusDeviceId)
        , position(aPosition)
    {
    }

    const char* eventName() const override { return kEventName; }
};

struct CoverTiltMotionChanged : common::domain::DomainEvent {
    static constexpr char kEventName[] = "cover_tilt_motion_changed";

    const EndpointId endpointId;
    const MobilusDeviceId mobilusDeviceId;
    const CoverMotion motion;

    CoverTiltMotionChanged(EndpointId aEndpointId, MobilusDeviceId aMobilusDeviceId, CoverMotion aMotion)
        : endpointId(aEndpointId)
        , mobilusDeviceId(aMobilusDeviceId)
        , motion(aMotion)
    {
    }

    const char* eventName() const override { return kEventName; }
};

struct CoverTiltRequested : common::domain::DomainEvent {
    static constexpr char kEventName[] = "cover_tilt_requested";

    const EndpointId endpointId;
    const MobilusDeviceId mobilusDeviceId;
    const Position position;

    CoverTiltRequested(EndpointId aEndpointId, MobilusDeviceId aMobilusDeviceId, Position aPosition)
        : endpointId(aEndpointId)
        , mobilusDeviceId(aMobilusDeviceId)
        , position(aPosition)
    {
    }

    const char* eventName() const override { return kEventName; }
};

struct CoverTiltTargetPositionChanged : common::domain::DomainEvent {
    static constexpr char kEventName[] = "cover_tilt_target_position_changed";

    const EndpointId endpointId;
    const MobilusDeviceId mobilusDeviceId;
    const Position position;

    CoverTiltTargetPositionChanged(EndpointId aEndpointId, MobilusDeviceId aMobilusDeviceId, Position aPosition)
        : endpointId(aEndpointId)
        , mobilusDeviceId(aMobilusDeviceId)
        , position(aPosition)
    {
    }

    const char* eventName() const override { return kEventName; }
};

}
