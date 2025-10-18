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
    FakeDeviceEventHandler(std::vector<mobgtw::proto::Event>& seenEvents, Result result = Result::Handled)
        : mSeenEvents(seenEvents)
        , mResult(result)
    {
    }

    Result handle(const mobgtw::proto::Event& event) override
    {
        mSeenEvents.push_back(event);

        return mResult;
    }

private:
    std::vector<mobgtw::proto::Event>& mSeenEvents;
    Result mResult;
};
