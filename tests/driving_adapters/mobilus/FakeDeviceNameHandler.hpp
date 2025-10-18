#pragma once

#include "driving_adapters/mobilus/MobilusDeviceNameHandler.h"

#include <unordered_map>

namespace model = mobmatter::application::model;

class FakeDeviceNameHandler final : public mobmatter::driving_adapters::mobilus::MobilusDeviceNameHandler {
public:
    std::unordered_map<model::MobilusDeviceId, std::string> handledNames;

    explicit FakeDeviceNameHandler(model::MobilusDeviceType deviceType)
        : mDeviceType(deviceType)
    {
    }

    void handle(model::MobilusDeviceId deviceId, const std::string& name)
    {
        handledNames[deviceId] = name;
    }

    bool supports(model::MobilusDeviceType deviceType) const { return deviceType == mDeviceType; }

private:
    model::MobilusDeviceType mDeviceType;
};
