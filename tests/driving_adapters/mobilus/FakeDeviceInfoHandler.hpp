#pragma once

#include "driving_adapters/mobilus/MobilusDeviceInfoHandler.h"

#include <vector>

namespace driving_adapter = mobmatter::driving_adapters::mobilus;
namespace proto = jungi::mobilus_gtw_client::proto;

class FakeDeviceInfoHandler final : public driving_adapter::MobilusDeviceInfoHandler {
public:
    explicit FakeDeviceInfoHandler(driving_adapter::HandlerResult result = driving_adapter::HandlerResult::Handled)
        : mResult(result)
    {
    }

    driving_adapter::HandlerResult handle(const proto::Device& deviceInfo) override
    {
        mSeenDevices.push_back(deviceInfo);

        return mResult;
    }

    const std::vector<proto::Device>& seenDevices() const { return mSeenDevices; }

private:
    std::vector<proto::Device> mSeenDevices;
    driving_adapter::HandlerResult mResult;
};
