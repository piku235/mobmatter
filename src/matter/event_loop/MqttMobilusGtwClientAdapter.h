#pragma once

#include "jungi/mobilus_gtw_client/io/ClientWatcher.h"
#include "matter/AppComponent.h"

#include <system/SocketEvents.h>
#include <system/SystemClock.h>
#include <system/SystemLayer.h>

namespace mmbridge::matter::event_loop {

namespace mobio = jungi::mobilus_gtw_client::io;

class MqttMobilusGtwClientAdapter final : public mobio::ClientWatcher,
                                          public chip::System::EventLoopHandler,
                                          public mmbridge::matter::AppComponent {
public:
    MqttMobilusGtwClientAdapter(chip::System::LayerSocketsLoop& systemLayer);
    ~MqttMobilusGtwClientAdapter();

    // ClientWatcher
    void watchTimer(mobio::TimerEventHandler* handler, std::chrono::milliseconds delay) override;
    void unwatchTimer(mobio::TimerEventHandler* handler) override;
    void watchSocket(mobio::SocketEventHandler* handler, int socketFd) override;
    void unwatchSocket(mobio::SocketEventHandler* handler, int socketFd) override;

    // AppComponent
    void shutdown() override;

    // EventLoopHandler
    chip::System::Clock::Timestamp PrepareEvents(chip::System::Clock::Timestamp now) override;
    void HandleEvents() override {};

private:
    mobio::TimerEventHandler* mTimerEventHandler = nullptr;
    mobio::SocketEventHandler* mSocketEventHandler = nullptr;
    chip::System::LayerSocketsLoop& mSystemLayer;
    chip::System::SocketWatchToken mSocketWatch = 0;

    static void handleSocketEventsCb(chip::System::SocketEvents events, intptr_t self) { reinterpret_cast<MqttMobilusGtwClientAdapter*>(self)->handleSocketEvents(events); }
    static void handleTimerEventCb(chip::System::Layer* layer, void* handler) { reinterpret_cast<mobio::TimerEventHandler*>(handler)->handleTimerEvent(); }

    void stopWatchingTimer();
    void stopWatchingSocket();
    void handleSocketEvents(chip::System::SocketEvents events);
};

}
