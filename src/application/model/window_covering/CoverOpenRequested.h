#pragma once

#include "application/model/EndpointId.h"
#include "application/model/MobilusDeviceId.h"
#include "common/domain/DomainEvent.h"

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

}
