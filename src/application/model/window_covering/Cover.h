#pragma once

#include "CoverMotionFault.h"
#include "CoverSpecification.h"
#include "Position.h"
#include "PositionState.h"
#include "application/model/Device.h"

namespace mobmatter::application::model::window_covering {

class Cover final : public Device {
public:
    [[nodiscard]] static Cover add(EndpointId endpointId, MobilusDeviceId mobilusDeviceId, CoverSpecification specification, std::string name, PositionState liftState, std::optional<PositionState> tiltState);
    [[nodiscard]] static Cover restoreFrom(EndpointId endpointId, MobilusDeviceId mobilusDeviceId, CoverSpecification specification, bool reachable, std::string name, std::optional<CoverMotionFault> motionFault, PositionState liftState, std::optional<PositionState> tiltState);

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
    Result reportAsReachable();
    Result reportAsUnreachable();
    Result reportMotionFault(CoverMotionFault fault);

    [[nodiscard]] bool isReachable() const { return mReachable; }
    [[nodiscard]] const CoverSpecification& specification() const { return mSpecification; }
    [[nodiscard]] std::optional<CoverMotionFault> motionFault() const { return mMotionFault; }
    [[nodiscard]] PositionState liftState() const { return mLiftState; }
    [[nodiscard]] std::optional<PositionState> tiltState() const { return mTiltState; }

private:
    /* const */ CoverSpecification mSpecification;
    bool mReachable;
    std::optional<CoverMotionFault> mMotionFault;
    PositionState mLiftState;
    std::optional<PositionState> mTiltState;

    Cover(EndpointId endpointId, MobilusDeviceId mobilusDeviceId, CoverSpecification specification, bool reachable, std::string name, std::optional<CoverMotionFault> motionFault, PositionState liftState, std::optional<PositionState> tiltState);
    Result changeLiftAndTiltTargetPosition(Position position);
    Result changeLiftTargetPosition(Position position);
    Result changeTiltTargetPosition(Position position);

    std::unique_ptr<common::domain::DomainEvent> deviceRemoved() override;
    std::unique_ptr<common::domain::DomainEvent> deviceRenamed() override;
};

}
