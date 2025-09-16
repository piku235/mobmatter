#pragma once

#include "Position.h"
#include "application/model/EndpointId.h"
#include "application/model/MobilusDeviceId.h"
#include "common/domain/DomainEvent.h"

namespace mobmatter::application::model::window_covering {

struct CoverLiftRequested : public mobmatter::common::domain::DomainEvent {
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

}
