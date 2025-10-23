#include "MobilusDeviceInfoSyncerAdapter.h"
#include "jungi/mobilus_gtw_client/EventNumber.h"

using namespace jungi::mobilus_gtw_client;

namespace mobmatter::driving_adapters::mobilus::device_handlers {

MobilusDeviceInfoSyncerAdapter::MobilusDeviceInfoSyncerAdapter(MqttMobilusDeviceInfoSyncer& syncer)
    : mSyncer(syncer)
{
}

HandlerResult MobilusDeviceInfoSyncerAdapter::handle(const proto::Event& event)
{
    if (EventNumber::Device == event.event_number() && "MODIFY" == event.value()) {
        mSyncer.run();

        return HandlerResult::Handled;
    }

    return HandlerResult::Unmatched;
}

}
