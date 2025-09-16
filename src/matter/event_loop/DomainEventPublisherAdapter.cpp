#include "DomainEventPublisherAdapter.h"
#include "common/domain/DomainEventPublisher.h"
#include "common/domain/DomainEventQueue.h"

using namespace mobmatter::common::domain;

namespace mobmatter::matter::event_loop {

DomainEventPublisherAdapter::DomainEventPublisherAdapter(chip::System::LayerSocketsLoop& systemLayer)
    : mSystemLayer(systemLayer)
{
}

DomainEventPublisherAdapter::~DomainEventPublisherAdapter()
{
    shutdown();
}

void DomainEventPublisherAdapter::boot()
{
    if (mRegistered) {
        return;
    }

    mSystemLayer.AddLoopHandler(*this);
    mRegistered = true;
}

void DomainEventPublisherAdapter::shutdown()
{
    if (!mRegistered) {
        return;
    }

    mSystemLayer.RemoveLoopHandler(*this);
    mRegistered = false;
}

void DomainEventPublisherAdapter::HandleEvents()
{
    DomainEventPublisher::instance().publish(DomainEventQueue::instance());
}

}
