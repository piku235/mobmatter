#include "CoverEndpointSubscriber.h"

using namespace mobmatter::application::model::window_covering;
using mobmatter::application::driven_ports::CoverEndpointService;

namespace mobmatter::application::subscribers {

CoverEndpointSubscriber::CoverEndpointSubscriber(CoverEndpointService& coverEndpointService)
    : mCoverEndpointService(coverEndpointService)
{
}

void CoverEndpointSubscriber::handle(const CoverAdded& event)
{
    mCoverEndpointService.addEndpoint(event.endpointId, event.specification);
}

void CoverEndpointSubscriber::handle(const CoverRemoved& event)
{
    mCoverEndpointService.removeEndpoint(event.endpointId);
}

}
