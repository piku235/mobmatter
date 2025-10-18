#pragma once

#include "driving_adapters/mobilus/MobilusDeviceEventHandler.h"
#include "driving_adapters/mobilus/MqttMobilusDeviceNameSyncer.h"

namespace mobmatter::driving_adapters::mobilus::generic {

class MobilusDeviceNameSyncerAdapter : public MobilusDeviceEventHandler {
public:
    explicit MobilusDeviceNameSyncerAdapter(MqttMobilusDeviceNameSyncer& mobilusDeviceNameSyncer);
    Result handle(const proto::Event& event) override;

private:
    MqttMobilusDeviceNameSyncer& mMobilusDeviceNameSyncer;
};

}
