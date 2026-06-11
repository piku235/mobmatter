#pragma once

#include "CoverMotion.h"
#include "application/model/EndpointId.h"
#include "application/model/MobilusDeviceId.h"
#include "common/domain/DomainEvent.h"

namespace mobmatter::application::model::window_covering {

struct CoverTiltMotionChanged : public mobmatter::common::domain::DomainEvent {
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

}
