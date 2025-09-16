#include "ChipCoverEndpointSubscriber.h"

using namespace mobmatter::application::model::window_covering;
using mobmatter::application::driven_ports::CoverEndpointService;

namespace mobmatter::application::subscribers {

ChipCoverEndpointSubscriber::ChipCoverEndpointSubscriber(CoverEndpointService& coverEndpointService)
    : mCoverEndpointService(coverEndpointService)
{
}

void ChipCoverEndpointSubscriber::handle(const CoverAdded& event)
{
    mCoverEndpointService.addEndpoint(event.endpointId, event.specification);
}

void ChipCoverEndpointSubscriber::handle(const CoverRemoved& event)
{
    mCoverEndpointService.removeEndpoint(event.endpointId);
}

}
