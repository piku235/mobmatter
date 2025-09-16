#pragma once

#include "application/model/EndpointId.h"
#include "application/model/MobilusDeviceId.h"
#include "common/domain/DomainEvent.h"

namespace mobmatter::application::model::window_covering {

struct CoverRemoved : public mobmatter::common::domain::DomainEvent {
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

}
