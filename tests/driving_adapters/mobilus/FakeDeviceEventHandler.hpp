#pragma once

#include "driving_adapters/mobilus/MobilusDeviceEventHandler.h"

#include <vector>

namespace driving_adapter = mobmatter::driving_adapters::mobilus;
namespace proto = jungi::mobilus_gtw_client::proto;

class FakeDeviceEventHandler final : public driving_adapter::MobilusDeviceEventHandler {
public:
    explicit FakeDeviceEventHandler(Result result)
        : mResult(result)
    {
    }

    Result handle(const proto::Event& event) override
    {
        mSeenEvents.push_back(event);

        return mResult;
    }

    const std::vector<proto::Event>& seenEvents() const { return mSeenEvents; }

private:
    std::vector<proto::Event> mSeenEvents;
    Result mResult;
};
