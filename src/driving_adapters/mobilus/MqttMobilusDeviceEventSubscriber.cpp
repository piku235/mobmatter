#include "MqttMobilusDeviceEventSubscriber.h"
#include "HandlerResult.h"
#include "jungi/mobilus_gtw_client/MessageType.h"

using namespace jungi::mobilus_gtw_client;

namespace mobmatter::driving_adapters::mobilus {

MqttMobilusDeviceEventSubscriber::MqttMobilusDeviceEventSubscriber(MqttMobilusGtwClient& client)
    : mClient(client)
{
}

void MqttMobilusDeviceEventSubscriber::registerHandler(MobilusDeviceEventHandler& handler)
{
    mHandlers.push_back(handler);
}

void MqttMobilusDeviceEventSubscriber::boot()
{
    mClient.messageBus().subscribe<proto::CallEvents>(MessageType::CallEvents, [this](auto& message) { handle(message); });
}

void MqttMobilusDeviceEventSubscriber::handle(const proto::CallEvents& callEvents)
{
    for (int i = 0; i < callEvents.events_size(); i++) {
        auto& event = callEvents.events(i);

        for (MobilusDeviceEventHandler& handler : mHandlers) {
            if (HandlerResult::Unmatched != handler.handle(event)) {
                break;
            }
        }
    }
}

}
