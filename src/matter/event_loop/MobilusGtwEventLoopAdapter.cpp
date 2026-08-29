#include "MobilusGtwEventLoopAdapter.h"

using namespace chip::System::Clock;
using chip::System::SocketEventFlags;

namespace mobmatter::matter::event_loop {

MobilusGtwEventLoopAdapter::MobilusGtwEventLoopAdapter(chip::System::LayerSelectLoop& selectLoop)
    : mSelectLoop(selectLoop)
{
}

void MobilusGtwEventLoopAdapter::boot()
{
    mSelectLoop.AddLoopHandler(*this);
}

void MobilusGtwEventLoopAdapter::shutdown()
{
    for (auto& [_, socketWatch] : mSocketWatchList) {
        (void)mSelectLoop.StopWatchingSocket(&socketWatch.token);
    }

    mSocketWatchList.clear();

    for (auto& timer : mTimers) {
        if (nullptr == timer.callback) {
            continue;
        }

        mSelectLoop.CancelTimer(timerCallback, &timer);
        timer = { };
    }

    mSelectLoop.RemoveLoopHandler(*this);
}

mobio::EventLoop::TimerId MobilusGtwEventLoopAdapter::startTimer(std::chrono::milliseconds delay, TimerCallback callback, void* callbackData)
{
    for (int i = 0; i < CHIP_SYSTEM_CONFIG_NUM_TIMERS; i++) {
        if (nullptr != mTimers[i].callback) {
            continue;
        }

        mTimers[i] = { callback, callbackData };
        mSelectLoop.StartTimer(delay, timerCallback, &mTimers[i]);

        return i;
    }

    return kInvalidTimerId;
}

void MobilusGtwEventLoopAdapter::stopTimer(TimerId id)
{
    if (id > kInvalidTimerId && id < CHIP_SYSTEM_CONFIG_NUM_TIMERS && nullptr != mTimers[id].callback) {
        mSelectLoop.CancelTimer(timerCallback, &mTimers[id]);
    }
}

void MobilusGtwEventLoopAdapter::watchSocket(int socketFd, mobio::SocketEventHandler* handler)
{
    auto& socketWatch = mSocketWatchList[socketFd];
    socketWatch.handler = handler;

    (void)mSelectLoop.StartWatchingSocket(socketFd, &socketWatch.token);
    (void)mSelectLoop.SetCallback(socketWatch.token, socketWatchCallback, reinterpret_cast<intptr_t>(handler));
}

void MobilusGtwEventLoopAdapter::unwatchSocket(int socketFd)
{
    if (auto it = mSocketWatchList.find(socketFd); it != mSocketWatchList.end()) {
        (void)mSelectLoop.StopWatchingSocket(&it->second.token);
        mSocketWatchList.erase(it);
    }
}

Timestamp MobilusGtwEventLoopAdapter::PrepareEvents(Timestamp now)
{
    for (auto& [_, socketWatch] : mSocketWatchList) {
        auto events = socketWatch.handler->socketEvents();

        if (events.has(mobio::SocketEvents::Read)) {
            (void)mSelectLoop.RequestCallbackOnPendingRead(socketWatch.token);
        } else {
            (void)mSelectLoop.ClearCallbackOnPendingRead(socketWatch.token);
        }

        if (events.has(mobio::SocketEvents::Write)) {
            (void)mSelectLoop.RequestCallbackOnPendingWrite(socketWatch.token);
        } else {
            (void)mSelectLoop.ClearCallbackOnPendingWrite(socketWatch.token);
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

    *timer = { };
    callback(callbackData);
}

}
