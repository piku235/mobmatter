#include "ChipCoverEndpointSubscriber.h"

using namespace mmbridge::application::model::window_covering;
using mmbridge::application::driven_ports::CoverEndpointService;

namespace mmbridge::application::subscribers {

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
