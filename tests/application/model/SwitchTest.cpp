#include "application/model/Switch.h"

#include "application/model/SwitchEvents.h"
#include "common/domain/DomainEventQueue.h"

#include <gtest/gtest.h>

using namespace mobmatter::common::domain;
using namespace mobmatter::application::model;

namespace {

auto switchStub()
{
    auto switch_ = Switch::add(1, 11, false, "switch");
    auto& events = DomainEventQueue::instance();

    events.clear();

    return switch_;
}

}

TEST(SwitchTest, AddsNew)
{
    auto switch_ = Switch::add(1, 11, true, "switch");
    auto& events = DomainEventQueue::instance();

    ASSERT_EQ(1, switch_.endpointId());
    ASSERT_EQ(11, switch_.mobilusDeviceId());
    ASSERT_TRUE(switch_.isReachable());
    ASSERT_TRUE(switch_.isOn());
    ASSERT_EQ("switch", switch_.name());

    ASSERT_EQ(1u, events.size());
    ASSERT_STREQ(SwitchAdded::kEventName, events.peek()->eventName());

    auto& event = static_cast<const SwitchAdded&>(*events.peek());

    ASSERT_EQ(switch_.endpointId(), event.endpointId);
    ASSERT_EQ(switch_.mobilusDeviceId(), event.mobilusDeviceId);

    events.pop();
}

TEST(SwitchTest, Restores)
{
    auto switch_ = Switch::restoreFrom(1, 11, false, false, "switch");
    auto& events = DomainEventQueue::instance();

    ASSERT_EQ(1, switch_.endpointId());
    ASSERT_EQ(11, switch_.mobilusDeviceId());
    ASSERT_FALSE(switch_.isReachable());
    ASSERT_FALSE(switch_.isOn());
    ASSERT_EQ("switch", switch_.name());

    ASSERT_TRUE(events.empty());
}

TEST(SwitchTest, RequestsOn)
{
    auto switch_ = switchStub();
    auto& events = DomainEventQueue::instance();

    auto result = switch_.requestOn();

    ASSERT_EQ(Switch::Result::Ok, result);
    ASSERT_TRUE(switch_.isOn());

    ASSERT_EQ(2u, events.size());
    ASSERT_STREQ(SwitchTurnedOn::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const SwitchTurnedOn&>(*events.peek());

        ASSERT_EQ(switch_.endpointId(), event.endpointId);
        ASSERT_EQ(switch_.mobilusDeviceId(), event.mobilusDeviceId);
    }

    events.pop();
    ASSERT_STREQ(SwitchTurnOnRequested::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const SwitchTurnOnRequested&>(*events.peek());

        ASSERT_EQ(switch_.endpointId(), event.endpointId);
        ASSERT_EQ(switch_.mobilusDeviceId(), event.mobilusDeviceId);
    }

    events.pop();

    ASSERT_EQ(Switch::Result::NoChange, switch_.requestOn());
    ASSERT_TRUE(events.empty());
}

TEST(SwitchTest, RequestsOff)
{
    auto switch_ = switchStub();
    auto& events = DomainEventQueue::instance();

    ASSERT_EQ(Switch::Result::Ok, switch_.requestOn());
    events.clear();

    auto result = switch_.requestOff();

    ASSERT_EQ(Switch::Result::Ok, result);
    ASSERT_FALSE(switch_.isOn());

    ASSERT_EQ(2u, events.size());
    ASSERT_STREQ(SwitchTurnedOff::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const SwitchTurnedOff&>(*events.peek());

        ASSERT_EQ(switch_.endpointId(), event.endpointId);
        ASSERT_EQ(switch_.mobilusDeviceId(), event.mobilusDeviceId);
    }

    events.pop();
    ASSERT_STREQ(SwitchTurnOffRequested::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const SwitchTurnOffRequested&>(*events.peek());

        ASSERT_EQ(switch_.endpointId(), event.endpointId);
        ASSERT_EQ(switch_.mobilusDeviceId(), event.mobilusDeviceId);
    }

    events.pop();

    ASSERT_EQ(Switch::Result::NoChange, switch_.requestOff());
    ASSERT_TRUE(events.empty());
}

TEST(SwitchTest, RequestsToggle)
{
    auto switch_ = switchStub();
    auto& events = DomainEventQueue::instance();

    {
        auto result = switch_.requestToggle();

        ASSERT_EQ(Switch::Result::Ok, result);
        ASSERT_TRUE(switch_.isOn());
    }

    {
        auto result = switch_.requestToggle();

        ASSERT_EQ(Switch::Result::Ok, result);
        ASSERT_FALSE(switch_.isOn());
    }

    ASSERT_EQ(4u, events.size());
    ASSERT_STREQ(SwitchTurnedOn::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const SwitchTurnedOn&>(*events.peek());

        ASSERT_EQ(switch_.endpointId(), event.endpointId);
        ASSERT_EQ(switch_.mobilusDeviceId(), event.mobilusDeviceId);
    }

    events.pop();
    ASSERT_STREQ(SwitchTurnOnRequested::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const SwitchTurnOnRequested&>(*events.peek());

        ASSERT_EQ(switch_.endpointId(), event.endpointId);
        ASSERT_EQ(switch_.mobilusDeviceId(), event.mobilusDeviceId);
    }

    events.pop();
    ASSERT_STREQ(SwitchTurnedOff::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const SwitchTurnedOff&>(*events.peek());

        ASSERT_EQ(switch_.endpointId(), event.endpointId);
        ASSERT_EQ(switch_.mobilusDeviceId(), event.mobilusDeviceId);
    }

    events.pop();
    ASSERT_STREQ(SwitchTurnOffRequested::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const SwitchTurnOffRequested&>(*events.peek());

        ASSERT_EQ(switch_.endpointId(), event.endpointId);
        ASSERT_EQ(switch_.mobilusDeviceId(), event.mobilusDeviceId);
    }

    events.pop();
}

TEST(SwitchTest, ReportsOn)
{
    auto switch_ = switchStub();
    auto& events = DomainEventQueue::instance();

    auto result = switch_.reportOn();

    ASSERT_EQ(Switch::Result::Ok, result);
    ASSERT_TRUE(switch_.isOn());

    ASSERT_EQ(1u, events.size());
    ASSERT_STREQ(SwitchTurnedOn::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const SwitchTurnedOn&>(*events.peek());

        ASSERT_EQ(switch_.endpointId(), event.endpointId);
        ASSERT_EQ(switch_.mobilusDeviceId(), event.mobilusDeviceId);
    }

    events.pop();

    ASSERT_EQ(Switch::Result::NoChange, switch_.requestOn());
    ASSERT_TRUE(events.empty());
}

TEST(SwitchTest, ReportOnDoesNothingAfterRequest)
{
    auto switch_ = switchStub();
    auto& events = DomainEventQueue::instance();

    ASSERT_EQ(Switch::Result::Ok, switch_.requestOn());
    events.clear();

    ASSERT_EQ(Switch::Result::NoChange, switch_.reportOn());
    ASSERT_TRUE(events.empty());
}

TEST(SwitchTest, ReportsOff)
{
    auto switch_ = switchStub();
    auto& events = DomainEventQueue::instance();

    ASSERT_EQ(Switch::Result::Ok, switch_.reportOn());
    events.clear();

    auto result = switch_.reportOff();

    ASSERT_EQ(Switch::Result::Ok, result);
    ASSERT_FALSE(switch_.isOn());

    ASSERT_EQ(1u, events.size());
    ASSERT_STREQ(SwitchTurnedOff::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const SwitchTurnedOff&>(*events.peek());

        ASSERT_EQ(switch_.endpointId(), event.endpointId);
        ASSERT_EQ(switch_.mobilusDeviceId(), event.mobilusDeviceId);
    }

    events.pop();

    ASSERT_EQ(Switch::Result::NoChange, switch_.requestOff());
    ASSERT_TRUE(events.empty());
}

TEST(SwitchTest, ReportOffDoesNothingAfterRequest)
{
    auto switch_ = switchStub();
    auto& events = DomainEventQueue::instance();

    ASSERT_EQ(Switch::Result::Ok, switch_.reportOn());
    ASSERT_EQ(Switch::Result::Ok, switch_.requestOff());
    events.clear();

    ASSERT_EQ(Switch::Result::NoChange, switch_.reportOff());
    ASSERT_TRUE(events.empty());
}

TEST(SwitchTest, ReportsUnreachable)
{
    auto switch_ = switchStub();
    auto& events = DomainEventQueue::instance();

    auto result = switch_.reportUnreachable();

    ASSERT_EQ(Switch::Result::Ok, result);
    ASSERT_FALSE(switch_.isReachable());

    ASSERT_EQ(1u, events.size());
    ASSERT_STREQ(SwitchMarkedAsUnreachable::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const SwitchMarkedAsUnreachable&>(*events.peek());

        ASSERT_EQ(switch_.endpointId(), event.endpointId);
        ASSERT_EQ(switch_.mobilusDeviceId(), event.mobilusDeviceId);
    }

    events.pop();

    ASSERT_EQ(Switch::Result::NoChange, switch_.reportUnreachable());
    ASSERT_TRUE(events.empty());
}

TEST(SwitchTest, ReportsReachable)
{
    auto switch_ = switchStub();
    auto& events = DomainEventQueue::instance();

    ASSERT_EQ(Switch::Result::Ok, switch_.reportUnreachable());
    events.clear();

    auto result = switch_.reportReachable();

    ASSERT_EQ(Switch::Result::Ok, result);
    ASSERT_TRUE(switch_.isReachable());

    ASSERT_EQ(1u, events.size());
    ASSERT_STREQ(SwitchMarkedAsReachable::kEventName, events.peek()->eventName());

    {
        auto& event = static_cast<const SwitchMarkedAsReachable&>(*events.peek());

        ASSERT_EQ(switch_.endpointId(), event.endpointId);
        ASSERT_EQ(switch_.mobilusDeviceId(), event.mobilusDeviceId);
    }

    events.pop();

    ASSERT_EQ(Switch::Result::NoChange, switch_.reportReachable());
    ASSERT_TRUE(events.empty());
}
