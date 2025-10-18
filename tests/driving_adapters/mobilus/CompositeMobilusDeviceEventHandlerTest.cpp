#include "driving_adapters/mobilus/CompositeMobilusDeviceEventHandler.hpp"
#include "driving_adapters/mobilus/FakeDeviceEventHandler.hpp"
#include "driving_adapters/mobilus/MobilusDeviceEventHandler.h"
#include "jungi/mobilus_gtw_client/proto/Event.pb.h"

#include <gtest/gtest.h>

#include <vector>

using namespace jungi::mobilus_gtw_client;
using mobmatter::driving_adapters::mobilus::CompositeMobilusDeviceEventHandler;
using mobmatter::driving_adapters::mobilus::MobilusDeviceEventHandler;

namespace {

auto eventStub()
{
    proto::Event event;

    event.set_device_id(2);
    event.set_value("0%");
    event.set_event_number(EventNumber::Sent);

    return event;
}

}

TEST(CompositeMobilusDeviceEventHandlerTest, UnsupportedDeviceEvent)
{
    CompositeMobilusDeviceEventHandler compositeHandler;

    auto r = compositeHandler.handle(eventStub());

    ASSERT_EQ(MobilusDeviceEventHandler::Result::Unsupported, r);
}

TEST(CompositeMobilusDeviceEventHandlerTest, DelegatesDeviceEvent)
{
    std::vector<proto::Event> handledEvents;
    std::vector<proto::Event> unsupportedEvents;
    std::vector<proto::Event> otherEvents;

    CompositeMobilusDeviceEventHandler compositeHandler;
    FakeDeviceEventHandler handlerUnsupported(unsupportedEvents, MobilusDeviceEventHandler::Result::Unsupported);
    FakeDeviceEventHandler handlerHandled(handledEvents);
    FakeDeviceEventHandler handlerOther(otherEvents, MobilusDeviceEventHandler::Result::Unsupported);

    compositeHandler.registerHandler(handlerUnsupported);
    compositeHandler.registerHandler(handlerHandled);
    compositeHandler.registerHandler(handlerOther);

    auto r = compositeHandler.handle(eventStub());

    ASSERT_EQ(MobilusDeviceEventHandler::Result::Handled, r);
    ASSERT_TRUE(otherEvents.empty());
    ASSERT_EQ(1, unsupportedEvents.size());
    ASSERT_EQ(1, handledEvents.size());
}
