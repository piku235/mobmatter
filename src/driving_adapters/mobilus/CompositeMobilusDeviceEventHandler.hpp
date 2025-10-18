#pragma once

#include "MobilusDeviceEventHandler.h"

#include <functional>
#include <vector>

namespace mobmatter::driving_adapters::mobilus {

class CompositeMobilusDeviceEventHandler final : public MobilusDeviceEventHandler {
public:
    void registerHandler(MobilusDeviceEventHandler& handler) { mHandlers.push_back(handler); }
    Result handle(const proto::Event& event) override
    {
        for (MobilusDeviceEventHandler& handler : mHandlers) {
            if (MobilusDeviceEventHandler::Result::Unsupported != handler.handle(event)) {
                break;
            }
        }
    }

private:
    std::vector<std::reference_wrapper<MobilusDeviceEventHandler>> mHandlers;
};

}
