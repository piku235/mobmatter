#pragma once

#include "application/model/EndpointId.h"
#include "application/model/MobilusDeviceId.h"
#include "application/model/Switch.h"

#include <optional>
#include <vector>

namespace mobmatter::application::driven_ports {

class SwitchRepository {
public:
    virtual ~SwitchRepository() = default;
    virtual void save(const model::Switch& switch_) = 0;
    virtual void remove(const model::Switch& switch_) = 0;
    virtual std::optional<model::Switch> findOfMobilusDeviceId(model::MobilusDeviceId deviceId) const = 0;
    virtual std::optional<model::Switch> find(model::EndpointId endpointId) const = 0;
    virtual std::vector<model::Switch> all() const = 0;
};

}
