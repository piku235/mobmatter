#pragma once

#include "CoverMotion.h"
#include "application/model/EndpointId.h"
#include "application/model/MobilusDeviceId.h"
#include "common/domain/DomainEvent.h"

namespace mobmatter::application::model::window_covering {

struct CoverLiftMotionChanged : public mobmatter::common::domain::DomainEvent {
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

}
