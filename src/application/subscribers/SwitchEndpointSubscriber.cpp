#include "SwitchEndpointSubscriber.h"

using namespace mobmatter::application::model;
using mobmatter::application::driven_ports::SwitchEndpointService;

namespace mobmatter::application::subscribers {

SwitchEndpointSubscriber::SwitchEndpointSubscriber(SwitchEndpointService& switchEndpointService)
    : mSwitchEndpointService(switchEndpointService)
{
}

void SwitchEndpointSubscriber::handle(const SwitchAdded& event)
{
    mSwitchEndpointService.addEndpoint(event.endpointId);
}

void SwitchEndpointSubscriber::handle(const SwitchRemoved& event)
{
    mSwitchEndpointService.removeEndpoint(event.endpointId);
}

}
