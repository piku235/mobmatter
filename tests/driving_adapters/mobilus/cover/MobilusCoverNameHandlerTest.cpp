#include "driving_adapters/mobilus/cover/MobilusCoverNameHandler.h"
#include "application/model/MobilusDeviceType.h"
#include "application/model/window_covering/CoverSpecification.h"
#include "application/model/window_covering/Position.h"
#include "application/model/window_covering/PositionState.h"
#include "common/logging/Logger.h"
#include "driven_adapters/persistence/in_memory/InMemoryCoverRepository.h"

#include <gtest/gtest.h>

using namespace mobmatter::tests::driven_adapters::persistence::in_memory;
using namespace mobmatter::application::model;
using namespace mobmatter::application::model::window_covering;
using mobmatter::common::logging::Logger;
using mobmatter::driving_adapters::mobilus::cover::MobilusCoverNameHandler;

namespace {

auto coverStub()
{
    return Cover::add(1, 2, "Foo", PositionState::at(Position::fullyOpen()), CoverSpecification::Senso());
}

}

TEST(MobilusCoverNameHandlerTest, SupportsAndDoesNotSupportDeviceType)
{
    InMemoryCoverRepository coverRepository;
    MobilusCoverNameHandler handler(coverRepository, Logger::noop());

    ASSERT_TRUE(handler.supports(MobilusDeviceType::Senso));
    ASSERT_TRUE(handler.supports(MobilusDeviceType::Cosmo));
    ASSERT_TRUE(handler.supports(MobilusDeviceType::Cmr));
    ASSERT_FALSE(handler.supports(MobilusDeviceType::Cgr));
    ASSERT_FALSE(handler.supports(MobilusDeviceType::Switch));
    ASSERT_FALSE(handler.supports(MobilusDeviceType::SwitchNp));
    ASSERT_FALSE(handler.supports(MobilusDeviceType::CosmoCzr));
    ASSERT_FALSE(handler.supports(MobilusDeviceType::CosmoMzr));
    ASSERT_FALSE(handler.supports(MobilusDeviceType::SensoZ));
}

TEST(MobilusCoverNameHandlerTest, RenamesCover)
{
    InMemoryCoverRepository coverRepository;
    MobilusCoverNameHandler handler(coverRepository, Logger::noop());

    coverRepository.save(coverStub());

    handler.handle(2, "Bar");
    auto cover = coverRepository.findOfMobilusDeviceId(2);

    ASSERT_TRUE(cover.has_value());
    ASSERT_EQ("Bar", cover->name());
}
