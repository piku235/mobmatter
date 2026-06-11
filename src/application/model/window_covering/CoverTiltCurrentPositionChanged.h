#pragma once

#include "Position.h"
#include "application/model/EndpointId.h"
#include "application/model/MobilusDeviceId.h"
#include "common/domain/DomainEvent.h"

namespace mobmatter::application::model::window_covering {

struct CoverTiltCurrentPositionChanged : public mobmatter::common::domain::DomainEvent {
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

}
