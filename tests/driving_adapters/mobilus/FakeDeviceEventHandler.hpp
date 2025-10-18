#pragma once

#include "driving_adapters/mobilus/MobilusDeviceEventHandler.h"
#include "driving_adapters/mobilus/MqttMobilusDeviceStateSyncer.h"
#include "jungi/mobilus_gtw_client/EventNumber.h"
#include "jungi/mobilus_gtw_client/proto/CallEvents.pb.h"
#include "mobilus/MockMqttMobilusGtwClient.hpp"

#include <vector>

namespace mobgtw = jungi::mobilus_gtw_client;

class FakeDeviceEventHandler final : public mobmatter::driving_adapters::mobilus::MobilusDeviceEventHandler {
public:
    FakeDeviceEventHandler(std::vector<mobgtw::proto::Event>& handledEvents)
        : mHandledEvents(handledEvents)
    {
    }

    Result handle(const mobgtw::proto::Event& event) override
    {
        mHandledEvents.push_back(event);

        return Result::Handled;
    }

private:
    std::vector<mobgtw::proto::Event>& mHandledEvents;
};
