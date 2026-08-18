#include "SwitchBasicInfoLoader.h"

#include <string>

using namespace mobmatter::driving_adapters::matter::bridged_device_cluster;
using mobmatter::application::driven_ports::SwitchRepository;

static constexpr char kProductName[] = "Switch";

namespace mobmatter::driving_adapters::matter::switch_cluster {

SwitchBasicInfoLoader::SwitchBasicInfoLoader(SwitchRepository& switchRepository)
    : mSwitchRepository(switchRepository)
{
}

std::optional<BridgedDeviceBasicInfoData> SwitchBasicInfoLoader::load(chip::EndpointId endpointId) const
{
    auto switch_ = mSwitchRepository.find(endpointId);
    if (!switch_) {
        return std::nullopt;
    }

    return BridgedDeviceBasicInfoData {
        switch_->endpointId(),
        std::to_string(switch_->mobilusDeviceId()),
        switch_->isReachable(),
        kProductName,
        switch_->name(),
    };
}

}
