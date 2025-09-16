#pragma once

#include "application/driven_ports/CoverRepository.h"
#include <unordered_map>

namespace mobmatter::tests::driven_adapters::persistence::in_memory {

namespace model = mobmatter::application::model;

class InMemoryCoverRepository : public mobmatter::application::driven_ports::CoverRepository {
public:
    void save(const model::window_covering::Cover& cover) override;
    void remove(const model::window_covering::Cover& cover) override;
    std::optional<model::window_covering::Cover> findOfMobilusDeviceId(model::MobilusDeviceId deviceId) const override;
    std::optional<model::window_covering::Cover> find(model::EndpointId endpointId) const override;
    std::vector<model::window_covering::Cover> all() const override;

private:
    std::unordered_map<model::EndpointId, model::window_covering::Cover> mCovers;
};

}
