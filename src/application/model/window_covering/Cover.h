#pragma once

#include "CoverOperationalStatus.h"
#include "CoverSpecification.h"
#include "Position.h"
#include "PositionState.h"
#include "application/model/EndpointId.h"
#include "application/model/MobilusDeviceId.h"
#include "application/model/UniqueId.h"
#include "common/domain/Entity.h"

#include <cstdint>
#include <optional>

namespace mmbridge::application::model::window_covering {

class Cover final : public mmbridge::common::domain::Entity {
public:
    static Cover add(EndpointId endpointId, MobilusDeviceId mobilusDeviceId, std::string name, PositionState liftState, CoverSpecification specification);
    static Cover restoreFrom(EndpointId endpointId, MobilusDeviceId mobilusDeviceId, UniqueId uniqueId, bool reachable, std::string name, PositionState liftState, CoverSpecification specification);

    /* chip specific */
    void requestOpen();
    void requestClose();
    void requestLiftTo(Position position);
    void requestStopMotion();

    /* mobilus specific */
    void initiateStopMotion();
    void startLiftTo(Position position);
    void changeLiftPosition(Position position);
    void failMotion();
    void markAsUnreachable();
    void remove();

    bool operator==(const Cover& other) const;
    bool isReachable() const;
    EndpointId endpointId() const;
    MobilusDeviceId mobilusDeviceId() const;
    const UniqueId& uniqueId() const;
    const std::string& name() const;
    CoverOperationalStatus operationalStatus() const;
    const PositionState& liftState() const;
    const CoverSpecification& specification() const;

private:
    /* const */ EndpointId mEndpointId;
    /* const */ MobilusDeviceId mMobilusDeviceId;
    /* const */ UniqueId mUniqueId;
    /* const */ CoverSpecification mSpecification;
    bool mReachable;
    std::string mName;
    PositionState mLiftState;

    Cover(EndpointId endpointId, MobilusDeviceId mobilusDeviceId, UniqueId uniqueId, bool reachable, std::string name, PositionState liftState, CoverSpecification specification);
    void replaceLiftState(PositionState&& liftState);
    void markAsReachable();
};

}
