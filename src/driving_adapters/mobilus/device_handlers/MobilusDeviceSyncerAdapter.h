#pragma once

#include "driving_adapters/mobilus/MobilusDeviceEventHandler.h"
#include "driving_adapters/mobilus/MqttMobilusDeviceSyncer.h"

namespace mobmatter::driving_adapters::mobilus::device_handlers {

class MobilusDeviceSyncerAdapter final : public MobilusDeviceEventHandler {
public:
    explicit MobilusDeviceSyncerAdapter(MqttMobilusDeviceSyncer& syncer);
    Result handle(const proto::Event& event) override;

private:
    MqttMobilusDeviceSyncer& mSyncer;
};

}
