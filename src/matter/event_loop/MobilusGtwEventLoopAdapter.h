#pragma once

#include "jungi/mobgtw/io/EventLoop.h"
#include "matter/AppComponent.h"

#include <system/SocketEvents.h>
#include <system/SystemClock.h>
#include <system/SystemConfig.h>
#include <system/SystemLayer.h>

#include <unordered_map>

namespace mobmatter::matter::event_loop {

namespace mobio = jungi::mobgtw::io;

class MobilusGtwEventLoopAdapter final : public mobio::EventLoop,
                                         public chip::System::EventLoopHandler,
                                         public mobmatter::matter::AppComponent {
public:
    MobilusGtwEventLoopAdapter(chip::System::LayerSocketsLoop& systemLayer);
    ~MobilusGtwEventLoopAdapter();

    // EventLoop
    TimerId startTimer(std::chrono::milliseconds delay, TimerCallback callback, void* callbackData) override;
    void stopTimer(TimerId id) override;
    void watchSocket(int socketFd, mobio::SocketEventHandler* handler) override;
    void unwatchSocket(int socketFd) override;

    // AppComponent
    void boot() override;
    void shutdown() override;

    // EventLoopHandler
    chip::System::Clock::Timestamp PrepareEvents(chip::System::Clock::Timestamp now) override;
    void HandleEvents() override { };

private:
    struct SocketWatch {
        chip::System::SocketWatchToken token;
        mobio::SocketEventHandler* handler;
    };

    struct Timer {
        TimerCallback callback = nullptr;
        void* callbackData;
    };

    chip::System::LayerSocketsLoop& mSystemLayer;
    std::unordered_map<int, SocketWatch> mSocketWatchList;
    Timer mTimers[CHIP_SYSTEM_CONFIG_NUM_TIMERS];
    bool mLoopHandlerRegistered = false;

    static void socketWatchCallback(chip::System::SocketEvents events, intptr_t data);
    static void timerCallback(chip::System::Layer* aLayer, void* appState);

    void stopWatchingSocket();
    void handleSocketEvents(chip::System::SocketEvents events);
};

}
