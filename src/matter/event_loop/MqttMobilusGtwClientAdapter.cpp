#include "MqttMobilusGtwClientAdapter.h"

using namespace chip::System::Clock;
using chip::System::SocketEventFlags;

namespace mmbridge::matter::event_loop {

MqttMobilusGtwClientAdapter::MqttMobilusGtwClientAdapter(chip::System::LayerSocketsLoop& systemLayer)
    : mSystemLayer(systemLayer)
{
}

MqttMobilusGtwClientAdapter::~MqttMobilusGtwClientAdapter()
{
    shutdown();
}

void MqttMobilusGtwClientAdapter::watchTimer(mobio::TimerEventHandler* handler, std::chrono::milliseconds delay)
{
    if (nullptr != mTimerEventHandler && mTimerEventHandler != handler) {
        return;
    }

    mSystemLayer.StartTimer(delay, handleTimerEventCb, handler);
    mTimerEventHandler = handler;
}

void MqttMobilusGtwClientAdapter::unwatchTimer(mobio::TimerEventHandler* handler)
{
    if (mTimerEventHandler != handler) {
        return;
    }

    stopWatchingTimer();
}

void MqttMobilusGtwClientAdapter::watchSocket(mobio::SocketEventHandler* handler, int socketFd)
{
    if (nullptr != mSocketEventHandler) {
        return;
    }

    mSystemLayer.AddLoopHandler(*this);
    mSystemLayer.StartWatchingSocket(socketFd, &mSocketWatch);
    mSystemLayer.SetCallback(mSocketWatch, handleSocketEventsCb, reinterpret_cast<intptr_t>(this));

    mSocketEventHandler = handler;
}

void MqttMobilusGtwClientAdapter::unwatchSocket(mobio::SocketEventHandler* handler, int socketFd)
{
    if (mSocketEventHandler != handler) {
        return;
    }

    stopWatchingSocket();
}

void MqttMobilusGtwClientAdapter::shutdown()
{
    stopWatchingSocket();
    stopWatchingTimer();
}

Timestamp MqttMobilusGtwClientAdapter::PrepareEvents(Timestamp now)
{
    if (nullptr != mSocketEventHandler) {
        auto events = mSocketEventHandler->socketEvents();

        if (events.has(mobio::SocketEvents::Read)) {
            mSystemLayer.RequestCallbackOnPendingRead(mSocketWatch);
        } else {
            mSystemLayer.ClearCallbackOnPendingRead(mSocketWatch);
        }

        if (events.has(mobio::SocketEvents::Write)) {
            mSystemLayer.RequestCallbackOnPendingWrite(mSocketWatch);
        } else {
            mSystemLayer.ClearCallbackOnPendingWrite(mSocketWatch);
        }
    }

    return Timestamp::max();
}

void MqttMobilusGtwClientAdapter::handleSocketEvents(chip::System::SocketEvents revents)
{
    mobio::SocketEvents mobRevents;

    if (revents.Has(SocketEventFlags::kRead)) {
        mobRevents.set(mobio::SocketEvents::Read);
    }
    if (revents.Has(SocketEventFlags::kWrite)) {
        mobRevents.set(mobio::SocketEvents::Write);
    }

    mSocketEventHandler->handleSocketEvents(mobRevents);
}

void MqttMobilusGtwClientAdapter::stopWatchingTimer()
{
    if (nullptr == mTimerEventHandler) {
        return;
    }

    mSystemLayer.CancelTimer(handleTimerEventCb, mTimerEventHandler);
    mTimerEventHandler = nullptr;
}

void MqttMobilusGtwClientAdapter::stopWatchingSocket()
{
    if (!mSocketWatch) {
        return;
    }

    mSystemLayer.RemoveLoopHandler(*this);
    mSystemLayer.StopWatchingSocket(&mSocketWatch);
    mSocketEventHandler = nullptr;
    mSocketWatch = 0;
}

}
