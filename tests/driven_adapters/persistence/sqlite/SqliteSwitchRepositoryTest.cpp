#include "driven_adapters/persistence/sqlite/SqliteSwitchRepository.h"
#include "SqliteDatabaseSchema.h"
#include "application/model/Switch.h"
#include "common/logging/Logger.h"
#include "common/persistence/sqlite/Connection.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace mobmatter::application::model;
using namespace mobmatter::driven_adapters::persistence::sqlite;
using namespace mobmatter::common::logging;

namespace {

auto switch_on()
{
    return Switch::add(1, 11, true, "switch on");
}

auto switch_off()
{
    return Switch::add(2, 12, false, "switch off");
}

}

class SqliteSwitchRepositoryTest : public ::testing::Test {
protected:
    sqlite::Connection conn;
    SqliteSwitchRepository switchRepository;

    SqliteSwitchRepositoryTest()
        : conn(*sqlite::Connection::inMemory())
        , switchRepository(conn, Logger::noop())
    {
        (void)conn.exec(kDatabaseSchema);
    }
};

TEST_F(SqliteSwitchRepositoryTest, Saves)
{
    auto switch_ = switch_on();

    switchRepository.save(switch_);
    auto savedSwitch = switchRepository.find(switch_.endpointId());

    ASSERT_TRUE(savedSwitch.has_value());
    ASSERT_EQ(switch_.endpointId(), savedSwitch->endpointId());
    ASSERT_EQ(switch_.mobilusDeviceId(), savedSwitch->mobilusDeviceId());
    ASSERT_EQ(switch_.isReachable(), savedSwitch->isReachable());
    ASSERT_EQ(switch_.name(), savedSwitch->name());
}

TEST_F(SqliteSwitchRepositoryTest, Removes)
{
    auto switch_ = switch_on();

    switchRepository.save(switch_);
    ASSERT_TRUE(switchRepository.find(switch_.endpointId()));

    switchRepository.remove(switch_);
    ASSERT_FALSE(switchRepository.find(switch_.endpointId()));
}

TEST_F(SqliteSwitchRepositoryTest, FindsAndDoesNotFindOfMobilusDeviceId)
{
    auto switch_ = switch_on();
    switchRepository.save(switch_);

    auto foundSwitch = switchRepository.findOfMobilusDeviceId(switch_.mobilusDeviceId());

    ASSERT_TRUE(foundSwitch.has_value());
    ASSERT_EQ(switch_, foundSwitch);

    ASSERT_FALSE(switchRepository.findOfMobilusDeviceId(123));
}

TEST_F(SqliteSwitchRepositoryTest, FindsAll)
{
    std::vector<EndpointId> expectedSwitches = { 1, 2 };

    switchRepository.save(switch_on());
    switchRepository.save(switch_off());

    auto switches = switchRepository.all();

    ASSERT_EQ(2u, switches.size());

    for (auto& switch_ : switches) {
        ASSERT_THAT(expectedSwitches, ::testing::Contains(switch_.endpointId()));
    }
}
