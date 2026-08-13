#include "Device.h"

namespace mobmatter::application::model {

Device::Device(EndpointId endpointId, MobilusDeviceId mobilusDeviceId, std::string name)
    : mEndpointId(endpointId)
    , mMobilusDeviceId(mobilusDeviceId)
    , mName(std::move(name))
{
}

Device::Result Device::requestRename(std::string name)
{
    auto result = rename(std::move(name));

    if (Result::Ok == result) {
        raise(deviceRenameRequested());
    }

    return result;
}

Device::Result Device::reportRenamedTo(std::string name)
{
    return rename(std::move(name));
}

void Device::reportRemoved()
{
    raise(deviceRemoved());
}

Device::Result Device::rename(std::string name)
{
    if (mName == name) {
        return Result::NoChange;
    }

    mName = std::move(name);
    raise(deviceRenamed());

    return Result::Ok;
}

}
