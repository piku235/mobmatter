#pragma once

#include "CoverSpecification.h"
#include "Position.h"
#include "PositionState.h"
#include "application/model/Device.h"

namespace mobmatter::application::model::window_covering {

class Cover final : public Device {
public:
    enum class Error {
        Unknown,
        Unreachable,
    };

    static Cover add(EndpointId endpointId, MobilusDeviceId mobilusDeviceId, CoverSpecification specification, std::string name, PositionState liftState, PositionState tiltState);
    static Cover restoreFrom(EndpointId endpointId, MobilusDeviceId mobilusDeviceId, CoverSpecification specification, bool reachable, std::string name, PositionState liftState, PositionState tiltState);

    /* chip oriented */
    Result requestOpen();
    Result requestClose();
    Result requestLiftTo(Position position);
    Result requestTiltTo(Position position);
    Result requestStopMotion();

    /* mobilus oriented */
    Result reportOpen();
    Result reportClose();
    Result reportLiftTo(Position position);
    Result reportLiftPosition(Position position);
    Result reportTiltTo(Position position);
    Result reportTiltPosition(Position position);
    Result reportStopMotion();
    Result reportReachable();
    Result reportError(Error error);

    bool isReachable() const { return mReachable; }
    const CoverSpecification& specification() const { return mSpecification; }
    const PositionState& liftState() const { return mLiftState; }
    const PositionState& tiltState() const { return mTiltState; }

private:
    /* const */ CoverSpecification mSpecification;
    bool mReachable;
    PositionState mLiftState;
    PositionState mTiltState;

    Cover(EndpointId endpointId, MobilusDeviceId mobilusDeviceId, CoverSpecification specification, bool reachable, std::string name, PositionState liftState, PositionState tiltState);
    Result changeLiftAndTiltTargetPosition(Position position);
    Result changeLiftTargetPosition(Position position);
    Result changeTiltTargetPosition(Position position);
    Result stopMotion();

    std::unique_ptr<common::domain::DomainEvent> deviceRemoved() override;
    std::unique_ptr<common::domain::DomainEvent> deviceRenamed() override;
    std::unique_ptr<common::domain::DomainEvent> deviceRenameRequested() override;
};

}
