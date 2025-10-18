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

namespace mobmatter::application::model::window_covering {

class Cover final : public mobmatter::common::domain::Entity {
public:
    enum class Result: uint8_t {
        Ok = 0,
        NoChange = 1,
        LiftNotSupported = 2,
    };

    static Cover add(EndpointId endpointId, MobilusDeviceId mobilusDeviceId, std::string name, PositionState liftState, CoverSpecification specification);
    static Cover restoreFrom(EndpointId endpointId, MobilusDeviceId mobilusDeviceId, UniqueId uniqueId, bool reachable, std::string name, PositionState liftState, CoverSpecification specification);

    /* chip specific */
    Result requestLiftTo(Position position);
    Result requestOpen();
    Result requestClose();
    Result requestStopMotion();
    Result requestRename(std::string name);

    /* mobilus specific */
    Result startLiftTo(Position position);
    Result changeLiftPosition(Position position);
    Result initiateStopMotion();
    Result failMotion();
    Result markAsUnreachable();
    Result rename(std::string name);
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
