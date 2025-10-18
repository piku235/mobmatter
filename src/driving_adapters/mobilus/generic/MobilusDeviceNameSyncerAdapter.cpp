#include "MobilusDeviceNameSyncerAdapter.h"
#include "jungi/mobilus_gtw_client/EventNumber.h"

using namespace jungi::mobilus_gtw_client;

namespace mobmatter::driving_adapters::mobilus::generic {

MobilusDeviceNameSyncerAdapter::MobilusDeviceNameSyncerAdapter(MqttMobilusDeviceNameSyncer& mobilusDeviceNameSyncer)
    : mMobilusDeviceNameSyncer(mobilusDeviceNameSyncer)
{
}

MobilusDeviceEventHandler::Result MobilusDeviceNameSyncerAdapter::handle(const proto::Event& event)
{
    if (EventNumber::Device == event.event_number() && "MODIFY" == event.value()) {
        mMobilusDeviceNameSyncer.run();

        return Result::Handled;
    }

    return Result::Unsupported;
}

}
