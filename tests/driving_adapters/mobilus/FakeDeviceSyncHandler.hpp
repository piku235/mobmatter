#pragma once

#include "driving_adapters/mobilus/MobilusDeviceSyncHandler.h"

#include <utility>
#include <vector>

namespace driving_adapter = mobmatter::driving_adapters::mobilus;
namespace proto = jungi::mobilus_gtw_client::proto;

class FakeDeviceSyncHandler final : public driving_adapter::MobilusDeviceSyncHandler {
public:
    explicit FakeDeviceSyncHandler(driving_adapter::HandlerResult result)
        : mResult(result)
    {
    }

    driving_adapter::HandlerResult handle(const proto::Device& deviceInfo, const proto::Event& currentState) override
    {
        mSeenDeviceEventPairs.push_back({ deviceInfo, currentState });

        return mResult;
    }

    const std::vector<std::pair<proto::Device, proto::Event>>& seenDeviceEventPairs() const { return mSeenDeviceEventPairs; }

private:
    std::vector<std::pair<proto::Device, proto::Event>> mSeenDeviceEventPairs;
    driving_adapter::HandlerResult mResult;
};
