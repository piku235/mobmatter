#include "DeviceManagementSubscriber.h"

using namespace mobmatter::application::model::window_covering;
using mobmatter::application::driven_ports::DeviceManagementService;

namespace mobmatter::application::subscribers {

DeviceManagementSubscriber::DeviceManagementSubscriber(DeviceManagementService& deviceManagementService)
    : mDeviceManagementService(deviceManagementService)
{
}

void DeviceManagementSubscriber::handle(const CoverRenameRequested& event)
{
    mDeviceManagementService.renameDevice(event.mobilusDeviceId, event.name);
}

}
