#include "MqttMobilusDeviceEventSubscriber.h"
#include "jungi/mobilus_gtw_client/MessageType.h"

using namespace jungi::mobilus_gtw_client;

namespace mobmatter::driving_adapters::mobilus {

MqttMobilusDeviceEventSubscriber::MqttMobilusDeviceEventSubscriber(MqttMobilusGtwClient& mobilusGtwClient, MobilusDeviceEventHandler& eventHandler)
    : mMobilusGtwClient(mobilusGtwClient)
    , mEventHandler(eventHandler)
{
}

void MqttMobilusDeviceEventSubscriber::boot()
{
    mMobilusGtwClient.messageBus().subscribe<proto::CallEvents>(MessageType::CallEvents, [this](auto& message) { handle(message); });
}

void MqttMobilusDeviceEventSubscriber::handle(const proto::CallEvents& callEvents)
{
    for (int i = 0; i < callEvents.events_size(); i++) {
        mEventHandler.handle(callEvents.events(i));
    }
}

}
