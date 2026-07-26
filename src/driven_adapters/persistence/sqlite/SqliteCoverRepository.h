#pragma once

#include "application/driven_ports/CoverRepository.h"
#include "common/logging/Logger.h"
#include "common/persistence/sqlite/Connection.h"

namespace mobmatter::driven_adapters::persistence::sqlite {

namespace model = application::model;
namespace sqlite = common::persistence::sqlite;
namespace logging = common::logging;

class SqliteCoverRepository final : public application::driven_ports::CoverRepository {
public:
    SqliteCoverRepository(sqlite::Connection& conn, logging::Logger& logger);
    void save(const model::window_covering::Cover& cover) override;
    void remove(const model::window_covering::Cover& cover) override;
    std::optional<model::window_covering::Cover> findOfMobilusDeviceId(model::MobilusDeviceId deviceId) const override;
    std::optional<model::window_covering::Cover> find(model::EndpointId endpointId) const override;
    std::vector<model::window_covering::Cover> all() const override;

private:
    sqlite::Connection& mConn;
    logging::Logger& mLogger;

    static model::window_covering::Cover mapRowTo(sqlite::Statement& stmt);
};

}
