#pragma once

#include "MobilusDeviceEventHandler.h"
#include "jungi/mobilus_gtw_client/MqttMobilusGtwClient.h"
#include "jungi/mobilus_gtw_client/proto/CallEvents.pb.h"
#include "matter/AppComponent.h"

#include <functional>
#include <vector>

namespace mobmatter::driving_adapters::mobilus {

namespace proto = jungi::mobilus_gtw_client::proto;

class MqttMobilusDeviceEventSubscriber final : public mobmatter::matter::AppComponent {
public:
    explicit MqttMobilusDeviceEventSubscriber(jungi::mobilus_gtw_client::MqttMobilusGtwClient& client);

    void registerHandler(MobilusDeviceEventHandler& handler);
    void boot() override;

private:
    jungi::mobilus_gtw_client::MqttMobilusGtwClient& mClient;
    std::vector<std::reference_wrapper<MobilusDeviceEventHandler>> mHandlers;

    void handle(const proto::CallEvents& callEvents);
};

}
