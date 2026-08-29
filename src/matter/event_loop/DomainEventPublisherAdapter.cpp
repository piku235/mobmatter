#include "DomainEventPublisherAdapter.h"
#include "common/domain/DomainEventPublisher.h"
#include "common/domain/DomainEventQueue.h"

using namespace mobmatter::common::domain;

namespace mobmatter::matter::event_loop {

DomainEventPublisherAdapter::DomainEventPublisherAdapter(chip::System::LayerSelectLoop& systemLayer)
    : mSelectLoop(systemLayer)
{
}

void DomainEventPublisherAdapter::boot()
{
    mSelectLoop.AddLoopHandler(*this);
}

void DomainEventPublisherAdapter::shutdown()
{
    mSelectLoop.RemoveLoopHandler(*this);
}

void DomainEventPublisherAdapter::HandleEvents()
{
    DomainEventPublisher::instance().publish(DomainEventQueue::instance());
}

}
