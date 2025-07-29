#pragma once

#include "application/model/EndpointId.h"
#include "application/model/MobilusDeviceId.h"
#include "application/model/window_covering/Cover.h"

#include <optional>
#include <vector>

namespace mmbridge::application::driven_ports {

namespace model = mmbridge::application::model;

class CoverRepository {
public:
    virtual ~CoverRepository() = default;
    virtual void save(const model::window_covering::Cover& cover) = 0;
    virtual void remove(const model::window_covering::Cover& cover) = 0;
    virtual std::optional<model::window_covering::Cover> findOfMobilusDeviceId(model::MobilusDeviceId deviceId) const = 0;
    virtual std::optional<model::window_covering::Cover> find(model::EndpointId endpointId) const = 0;
    virtual std::vector<model::window_covering::Cover> all() const = 0;
};

}
