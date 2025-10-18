#pragma once

#include "application/model/EndpointId.h"
#include "application/model/MobilusDeviceId.h"
#include "common/domain/DomainEvent.h"

#include <string>

namespace mobmatter::application::model::window_covering {

struct CoverRenameRequested : public mobmatter::common::domain::DomainEvent {
    static constexpr char kEventName[] = "cover_rename_requested";

    const EndpointId endpointId;
    const MobilusDeviceId mobilusDeviceId;
    const std::string name;

    CoverRenameRequested(EndpointId aEndpointId, MobilusDeviceId aMobilusDeviceId, std::string aName)
        : endpointId(aEndpointId)
        , mobilusDeviceId(aMobilusDeviceId)
        , name(std::move(aName))
    {
    }

    const char* eventName() const override { return kEventName; }
};

}
