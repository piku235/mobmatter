#include "DomainEventPublisherAdapter.h"
#include "common/domain/DomainEventPublisher.h"
#include "common/domain/DomainEventQueue.h"

using namespace mobmatter::common::domain;

namespace mobmatter::matter::event_loop {

DomainEventPublisherAdapter::DomainEventPublisherAdapter(chip::System::LayerSocketsLoop& systemLayer)
    : mSystemLayer(systemLayer)
{
}

void DomainEventPublisherAdapter::boot()
{
    mSystemLayer.AddLoopHandler(*this);
}

void DomainEventPublisherAdapter::shutdown()
{
    mSystemLayer.RemoveLoopHandler(*this);
}

void DomainEventPublisherAdapter::HandleEvents()
{
    DomainEventPublisher::instance().publish(DomainEventQueue::instance());
}

}
