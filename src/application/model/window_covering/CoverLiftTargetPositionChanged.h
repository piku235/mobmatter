#pragma once

#include "Position.h"
#include "PositionStatus.h"
#include "application/model/EndpointId.h"
#include "application/model/MobilusDeviceId.h"
#include "common/domain/DomainEvent.h"

namespace mobmatter::application::model::window_covering {

struct CoverLiftTargetPositionChanged : public mobmatter::common::domain::DomainEvent {
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

}
