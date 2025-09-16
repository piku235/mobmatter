#pragma once

#include "application/model/EndpointId.h"
#include "application/model/MobilusDeviceId.h"
#include "common/domain/DomainEvent.h"

namespace mobmatter::application::model::window_covering {

struct CoverMarkedAsReachable : public mobmatter::common::domain::DomainEvent {
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

}
