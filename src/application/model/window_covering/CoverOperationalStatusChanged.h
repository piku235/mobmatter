#pragma once

#include "CoverOperationalStatus.h"
#include "application/model/EndpointId.h"
#include "application/model/MobilusDeviceId.h"
#include "common/domain/DomainEvent.h"

namespace mobmatter::application::model::window_covering {

struct CoverOperationalStatusChanged : public mobmatter::common::domain::DomainEvent {
    static constexpr char kEventName[] = "cover_operational_status_changed";

    const EndpointId endpointId;
    const MobilusDeviceId mobilusDeviceId;
    const CoverOperationalStatus operationalStatus;

    CoverOperationalStatusChanged(EndpointId aEndpointId, MobilusDeviceId aMobilusDeviceId, CoverOperationalStatus aOperationalStatus)
        : endpointId(aEndpointId)
        , mobilusDeviceId(aMobilusDeviceId)
        , operationalStatus(aOperationalStatus)
    {
    }

    const char* eventName() const override { return kEventName; }
};

}
