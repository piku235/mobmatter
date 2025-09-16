#pragma once

#include "application/model/EndpointId.h"
#include "application/model/MobilusDeviceId.h"
#include "application/model/window_covering/CoverSpecification.h"
#include "common/domain/DomainEvent.h"

namespace mobmatter::application::model::window_covering {

struct CoverAdded : public mobmatter::common::domain::DomainEvent {
    static constexpr char kEventName[] = "cover_added";

    const EndpointId endpointId;
    const MobilusDeviceId mobilusDeviceId;
    const CoverSpecification specification;

    CoverAdded(EndpointId aEndpointId, MobilusDeviceId aMobilusDeviceId, CoverSpecification aSpecification)
        : endpointId(aEndpointId)
        , mobilusDeviceId(aMobilusDeviceId)
        , specification(aSpecification)
    {
    }

    const char* eventName() const override { return kEventName; }
};

}
