#pragma once

#include "MobilusDeviceEventHandler.h"
#include "jungi/mobilus_gtw_client/MqttMobilusGtwClient.h"
#include "jungi/mobilus_gtw_client/proto/CallEvents.pb.h"
#include "matter/AppComponent.h"

namespace mobmatter::driving_adapters::mobilus {

namespace proto = jungi::mobilus_gtw_client::proto;

class MqttMobilusDeviceEventSubscriber final : public mobmatter::matter::AppComponent {
public:
    MqttMobilusDeviceEventSubscriber(jungi::mobilus_gtw_client::MqttMobilusGtwClient& mobilusGtwClient, MobilusDeviceEventHandler& eventHandler);

    void boot() override;
    void handle(const proto::CallEvents& callEvents);

private:
    jungi::mobilus_gtw_client::MqttMobilusGtwClient& mMobilusGtwClient;
    MobilusDeviceEventHandler& mEventHandler;
};

}
