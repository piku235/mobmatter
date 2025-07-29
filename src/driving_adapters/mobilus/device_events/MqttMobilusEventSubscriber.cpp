#include "MqttMobilusEventSubscriber.h"
#include "jungi/mobilus_gtw_client/MessageType.h"

using namespace jungi::mobilus_gtw_client;

namespace mmbridge::driving_adapters::mobilus::device_events {

MqttMobilusEventSubscriber::MqttMobilusEventSubscriber(MqttMobilusGtwClient& mobilusGtwClient, MobilusEventHandler& eventHandler)
    : mMobilusGtwClient(mobilusGtwClient)
    , mEventHandler(eventHandler)
{
}

void MqttMobilusEventSubscriber::boot()
{
    mMobilusGtwClient.messageBus().subscribe<proto::CallEvents>(MessageType::CallEvents, [this](auto& message) { handle(message); });
}

void MqttMobilusEventSubscriber::handle(const proto::CallEvents& callEvents)
{
    for (int i = 0; i < callEvents.events_size(); i++) {
        mEventHandler.handle(callEvents.events(i));
    }
}

}
