#include "MobilusGtwEventLoopAdapter.h"

using namespace chip::System::Clock;
using chip::System::SocketEventFlags;

namespace mmbridge::matter::event_loop {

MobilusGtwEventLoopAdapter::MobilusGtwEventLoopAdapter(chip::System::LayerSocketsLoop& systemLayer)
    : mSystemLayer(systemLayer)
{
}

MobilusGtwEventLoopAdapter::~MobilusGtwEventLoopAdapter()
{
    shutdown();
}

void MobilusGtwEventLoopAdapter::boot()
{
    if (!mLoopHandlerRegistered) {
        mSystemLayer.AddLoopHandler(*this);
        mLoopHandlerRegistered = true;
    }
}

void MobilusGtwEventLoopAdapter::shutdown()
{
    if (mLoopHandlerRegistered) {
        mSystemLayer.RemoveLoopHandler(*this);
        mLoopHandlerRegistered = false;
    }
}

mobio::EventLoop::TimerId MobilusGtwEventLoopAdapter::startTimer(std::chrono::milliseconds delay, TimerCallback callback, void* callbackData)
{
    for (int i = 0; i < CHIP_SYSTEM_CONFIG_NUM_TIMERS; i++) {
        if (nullptr != mTimers[i].callback) {
            continue;
        }

        mTimers[i] = { callback, callbackData };
        mSystemLayer.StartTimer(delay, timerCallback, &mTimers[i]);

        return i;
    }

    return kInvalidTimerId;
}

void MobilusGtwEventLoopAdapter::stopTimer(TimerId id)
{
    if (id > kInvalidTimerId && id < CHIP_SYSTEM_CONFIG_NUM_TIMERS && nullptr != mTimers[id].callback) {
        mSystemLayer.CancelTimer(timerCallback, &mTimers[id]);
    }
}

void MobilusGtwEventLoopAdapter::watchSocket(int socketFd, mobio::SocketEventHandler* handler)
{
    auto& socketWatch = mSocketWatchList[socketFd];
    socketWatch.handler = handler;

    mSystemLayer.StartWatchingSocket(socketFd, &socketWatch.token);
    mSystemLayer.SetCallback(socketWatch.token, socketWatchCallback, reinterpret_cast<intptr_t>(handler));
}

void MobilusGtwEventLoopAdapter::unwatchSocket(int socketFd)
{
    if (auto it = mSocketWatchList.find(socketFd); it != mSocketWatchList.end()) {
        mSystemLayer.StopWatchingSocket(&it->second.token);
        mSocketWatchList.erase(it);
    }
}

Timestamp MobilusGtwEventLoopAdapter::PrepareEvents(Timestamp now)
{
    for (auto& [_, socketWatch] : mSocketWatchList) {
        auto events = socketWatch.handler->socketEvents();

        if (events.has(mobio::SocketEvents::Read)) {
            mSystemLayer.RequestCallbackOnPendingRead(socketWatch.token);
        } else {
            mSystemLayer.ClearCallbackOnPendingRead(socketWatch.token);
        }

        if (events.has(mobio::SocketEvents::Write)) {
            mSystemLayer.RequestCallbackOnPendingWrite(socketWatch.token);
        } else {
            mSystemLayer.ClearCallbackOnPendingWrite(socketWatch.token);
        }
    }

    return Timestamp::max();
}

void MobilusGtwEventLoopAdapter::socketWatchCallback(chip::System::SocketEvents revents, intptr_t data)
{
    mobio::SocketEvents mobRevents;
    auto handler = reinterpret_cast<mobio::SocketEventHandler*>(data);

    if (revents.Has(SocketEventFlags::kRead)) {
        mobRevents.set(mobio::SocketEvents::Read);
    }
    if (revents.Has(SocketEventFlags::kWrite)) {
        mobRevents.set(mobio::SocketEvents::Write);
    }

    handler->handleSocketEvents(mobRevents);
}

void MobilusGtwEventLoopAdapter::timerCallback(chip::System::Layer* aLayer, void* appState)
{
    auto timer = reinterpret_cast<Timer*>(appState);
    auto callback = timer->callback;
    auto callbackData = timer->callbackData;

    *timer = {};
    callback(callbackData);
}

}
