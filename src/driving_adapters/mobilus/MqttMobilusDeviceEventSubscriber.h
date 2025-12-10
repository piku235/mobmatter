#pragma once

#include "MobilusDeviceEventHandler.h"
#include "matter/AppComponent.h"

#include <jungi/mobgtw/MqttMobilusGtwClient.h>
#include <jungi/mobgtw/proto/CallEvents.pb.h>

#include <functional>
#include <vector>

namespace mobmatter::driving_adapters::mobilus {

namespace proto = jungi::mobgtw::proto;

class MqttMobilusDeviceEventSubscriber final : public mobmatter::matter::AppComponent {
public:
    explicit MqttMobilusDeviceEventSubscriber(jungi::mobgtw::MqttMobilusGtwClient& client);

    void registerHandler(MobilusDeviceEventHandler& handler);
    void boot() override;

private:
    jungi::mobgtw::MqttMobilusGtwClient& mClient;
    std::vector<std::reference_wrapper<MobilusDeviceEventHandler>> mHandlers;

    void handle(const proto::CallEvents& callEvents);
};

}
