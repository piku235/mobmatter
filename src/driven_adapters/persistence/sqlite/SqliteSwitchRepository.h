#pragma once

#include "application/driven_ports/SwitchRepository.h"
#include "common/logging/Logger.h"
#include "common/persistence/sqlite/Connection.h"

namespace mobmatter::driven_adapters::persistence::sqlite {

namespace model = application::model;
namespace sqlite = common::persistence::sqlite;
namespace logging = common::logging;

class SqliteSwitchRepository final : public application::driven_ports::SwitchRepository {
public:
    SqliteSwitchRepository(sqlite::Connection& conn, logging::Logger& logger);
    void save(const model::Switch& switch_) override;
    void remove(const model::Switch& switch_) override;
    std::optional<model::Switch> findOfMobilusDeviceId(model::MobilusDeviceId deviceId) const override;
    std::optional<model::Switch> find(model::EndpointId endpointId) const override;
    std::vector<model::Switch> all() const override;

private:
    sqlite::Connection& mConn;
    logging::Logger& mLogger;

    static model::Switch mapRowTo(sqlite::Statement& stmt);
};

}
