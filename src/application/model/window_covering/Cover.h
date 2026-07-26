#pragma once

#include "CoverSpecification.h"
#include "Position.h"
#include "PositionState.h"
#include "application/model/EndpointId.h"
#include "application/model/MobilusDeviceId.h"
#include "application/model/UniqueId.h"
#include "common/domain/Entity.h"

namespace mobmatter::application::model::window_covering {

class Cover final : public common::domain::Entity {
public:
    enum class Result {
        Ok,
        NoChange,
        NotSupported,
    };
    enum class Error {
        Unknown,
        Unreachable,
    };

    static Cover add(EndpointId endpointId, MobilusDeviceId mobilusDeviceId, CoverSpecification specification, std::string name, PositionState liftState, PositionState tiltState);
    static Cover restoreFrom(EndpointId endpointId, MobilusDeviceId mobilusDeviceId, UniqueId uniqueId, CoverSpecification specification, bool reachable, std::string name, PositionState liftState, PositionState tiltState);

    /* chip specific */
    Result requestOpen();
    Result requestClose();
    Result requestLiftTo(Position position);
    Result requestTiltTo(Position position);
    Result requestStopMotion();
    Result requestRename(std::string name);

    /* mobilus specific */
    Result reportOpen();
    Result reportClose();
    Result reportLiftTo(Position position);
    Result reportLiftPosition(Position position);
    Result reportTiltTo(Position position);
    Result reportTiltPosition(Position position);
    Result reportStopMotion();
    Result reportReachable();
    Result reportError(Error error);
    Result reportRenamedTo(std::string name);
    void reportRemoved();

    bool operator==(const Cover& other) const;
    bool isReachable() const;
    EndpointId endpointId() const;
    MobilusDeviceId mobilusDeviceId() const;
    const UniqueId& uniqueId() const;
    const CoverSpecification& specification() const;
    const std::string& name() const;
    const PositionState& liftState() const;
    const PositionState& tiltState() const;

private:
    /* const */ EndpointId mEndpointId;
    /* const */ MobilusDeviceId mMobilusDeviceId;
    /* const */ UniqueId mUniqueId;
    /* const */ CoverSpecification mSpecification;
    bool mReachable;
    std::string mName;
    PositionState mLiftState;
    PositionState mTiltState;

    Cover(EndpointId endpointId, MobilusDeviceId mobilusDeviceId, UniqueId uniqueId, CoverSpecification specification, bool reachable, std::string name, PositionState liftState, PositionState tiltState);
    Result rename(std::string name);
    Result changeLiftAndTiltTargetPosition(Position position);
    Result changeLiftTargetPosition(Position position);
    Result changeTiltTargetPosition(Position position);
    Result stopMotion();
};

}
