#include "MqttMobilusDeviceEventSubscriber.h"

using namespace jungi::mobgtw;

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
    mClient.messageBus().subscribe<proto::CallEvents>([this](const auto& message) { handle(message); });
}

void MqttMobilusDeviceEventSubscriber::handle(const proto::CallEvents& callEvents)
{
    for (int i = 0; i < callEvents.events_size(); i++) {
        auto& event = callEvents.events(i);

        for (MobilusDeviceEventHandler& handler : mHandlers) {
            if (MobilusDeviceEventHandler::Result::Unmatched != handler.handle(event)) {
                break;
            }
        }
    }
}

}
