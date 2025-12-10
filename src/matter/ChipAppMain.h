#pragma once

#include "AppComponent.h"
#include "common/logging/Logger.h"

#include <jungi/mobgtw/MqttMobilusGtwClient.h>
#include <lib/core/CHIPError.h>
#include <lib/core/CHIPPersistentStorageDelegate.h>
#include <platform/DeviceInstanceInfoProvider.h>

#include <vector>

namespace mobmatter::matter {

class ChipAppMain final {
public:
    void registerComponent(AppComponent& component) { mComponents.push_back(&component); }
    int boot(mobmatter::common::logging::Logger& logger, jungi::mobgtw::MqttMobilusGtwClient& mobilusGtwClient, chip::PersistentStorageDelegate& persistentStorageDelegate);
    void run();
    void shutdown();

private:
    std::vector<AppComponent*> mComponents;
};

}
