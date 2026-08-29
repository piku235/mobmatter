#pragma once

#include "EndpointId.h"
#include "MobilusDeviceId.h"
#include "common/domain/DomainEvent.h"
#include "common/domain/Entity.h"

#include <memory>
#include <string>

namespace mobmatter::application::model {

class Device : public common::domain::Entity {
public:
    enum class [[nodiscard]] Result {
        Ok,
        NoChange,
        NotSupported,
    };

    Device(EndpointId endpointId, MobilusDeviceId mobilusDeviceId, std::string name);
    virtual ~Device() = default;

    Result reportRenamedTo(std::string name);
    void reportRemoved();

    bool operator==(const Device& other) const { return mEndpointId == other.mEndpointId; }
    [[nodiscard]] EndpointId endpointId() const { return mEndpointId; }
    [[nodiscard]] MobilusDeviceId mobilusDeviceId() const { return mMobilusDeviceId; }
    [[nodiscard]] const std::string& name() const { return mName; }

protected:
    /* const */ EndpointId mEndpointId;
    /* const */ MobilusDeviceId mMobilusDeviceId;
    std::string mName;

private:
    Result rename(std::string name);

    virtual std::unique_ptr<common::domain::DomainEvent> deviceRemoved() = 0;
    virtual std::unique_ptr<common::domain::DomainEvent> deviceRenamed() = 0;
};

}
