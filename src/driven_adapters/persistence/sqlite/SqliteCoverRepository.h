#pragma once

#include "application/driven_ports/CoverRepository.h"
#include "common/persistence/sqlite/Connection.h"

namespace mmbridge::driven_adapters::persistence::sqlite {

namespace model = mmbridge::application::model;
namespace sqlite = mmbridge::common::persistence::sqlite;

class SqliteCoverRepository final : public mmbridge::application::driven_ports::CoverRepository {
public:
    SqliteCoverRepository(mmbridge::common::persistence::sqlite::Connection& conn);
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
