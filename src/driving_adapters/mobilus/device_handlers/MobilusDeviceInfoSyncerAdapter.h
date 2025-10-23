#pragma once

#include "driving_adapters/mobilus/MobilusDeviceEventHandler.h"
#include "driving_adapters/mobilus/MqttMobilusDeviceInfoSyncer.h"

namespace mobmatter::driving_adapters::mobilus::device_handlers {

class MobilusDeviceInfoSyncerAdapter : public MobilusDeviceEventHandler {
public:
    explicit MobilusDeviceInfoSyncerAdapter(MqttMobilusDeviceInfoSyncer& syncer);
    HandlerResult handle(const proto::Event& event) override;

private:
    MqttMobilusDeviceInfoSyncer& mSyncer;
};

}
