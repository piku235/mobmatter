#pragma once

#include "EndpointId.h"
#include "MobilusDeviceId.h"
#include "common/domain/Entity.h"

#include <string>

namespace mobmatter::application::model {

class Switch final : public common::domain::Entity {
public:
    enum class Result {
        Ok,
        NoChange,
    };
    enum class Error {
        Unknown,
        Unreachable,
    };
    enum class State {
        On,
        Off,
        Unreachable,
    };

    static Switch add(EndpointId endpointId, MobilusDeviceId mobilusDeviceId, std::string name, State state);
    static Switch restoreFrom(EndpointId endpointId, MobilusDeviceId mobilusDeviceId, std::string name, State state);

    /* chip oriented */
    Result requestOn();
    Result requestOff();
    Result requestToggle();
    Result requestRename(std::string name);

    /* mobilus oriented */
    Result reportOn();
    Result reportOff();
    Result reportError(Error error);
    Result reportRenamedTo(std::string name);
    void reportRemoved();

    bool operator==(const Switch& other) const;
    EndpointId endpointId() const;
    MobilusDeviceId mobilusDeviceId() const;
    const std::string& name() const;
    State state() const;

private:
    /* const */ EndpointId mEndpointId;
    /* const */ MobilusDeviceId mMobilusDeviceId;
    std::string mName;
    State mState;

    Switch(EndpointId endpointId, MobilusDeviceId mobilusDeviceId, std::string name, State state);
    Result rename(std::string name);
    Result turnOn();
    Result turnOff();
};

}
