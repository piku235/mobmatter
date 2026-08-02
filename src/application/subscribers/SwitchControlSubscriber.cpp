#include "SwitchControlSubscriber.h"

namespace mobmatter::application::subscribers {

SwitchControlSubscriber::SwitchControlSubscriber(driven_ports::SwitchControlService& switchControlService)
    : mSwitchControlService(switchControlService)
{
}

void SwitchControlSubscriber::handle(const model::SwitchTurnOnRequested& event)
{
    mSwitchControlService.turnSwitchOn(event.mobilusDeviceId);
}

void SwitchControlSubscriber::handle(const model::SwitchTurnOffRequested& event)
{
    mSwitchControlService.turnSwitchOff(event.mobilusDeviceId);
}

}
