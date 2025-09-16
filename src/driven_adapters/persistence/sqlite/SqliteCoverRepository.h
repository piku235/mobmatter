#pragma once

#include "application/driven_ports/CoverRepository.h"
#include "common/persistence/sqlite/Connection.h"

namespace mobmatter::driven_adapters::persistence::sqlite {

namespace model = mobmatter::application::model;
namespace sqlite = mobmatter::common::persistence::sqlite;

class SqliteCoverRepository final : public mobmatter::application::driven_ports::CoverRepository {
public:
    SqliteCoverRepository(mobmatter::common::persistence::sqlite::Connection& conn);
    void save(const model::window_covering::Cover& cover) override;
    void remove(const model::window_covering::Cover& cover) override;
    std::optional<model::window_covering::Cover> findOfMobilusDeviceId(model::MobilusDeviceId deviceId) const override;
    std::optional<model::window_covering::Cover> find(model::EndpointId endpointId) const override;
    std::vector<model::window_covering::Cover> all() const override;

private:
    sqlite::Connection& mConn;

    model::window_covering::Cover mapRowTo(sqlite::Statement& stmt) const;
};

}
