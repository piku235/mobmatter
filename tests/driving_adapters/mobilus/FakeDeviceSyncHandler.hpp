#pragma once

#include "driving_adapters/mobilus/MobilusDeviceSyncHandler.h"

#include <utility>
#include <vector>

namespace driving_adapter = mobmatter::driving_adapters::mobilus;
namespace proto = jungi::mobgtw::proto;

class FakeDeviceSyncHandler final : public driving_adapter::MobilusDeviceSyncHandler {
public:
    void sync(const driving_adapter::DeviceStateMap& devices) override
    {
        mDevices = devices;
    }

    const driving_adapter::DeviceStateMap& syncedDevices() const { return mDevices; }

private:
    driving_adapter::DeviceStateMap mDevices;
};
