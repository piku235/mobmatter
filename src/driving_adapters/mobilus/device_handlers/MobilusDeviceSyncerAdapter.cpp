#include "MobilusDeviceSyncerAdapter.h"
#include "jungi/mobilus_gtw_client/EventNumber.h"

using namespace jungi::mobilus_gtw_client;

namespace mobmatter::driving_adapters::mobilus::device_handlers {

MobilusDeviceSyncerAdapter::MobilusDeviceSyncerAdapter(MqttMobilusDeviceSyncer& syncer)
    : mSyncer(syncer)
{
}

MobilusDeviceEventHandler::Result MobilusDeviceSyncerAdapter::handle(const proto::Event& event)
{
    if (EventNumber::Device == event.event_number() && ("ADD" == event.value() || "MODIFY" == event.value() || "REMOVE" == event.value())) {
        mSyncer.run();
    }
}

}
