#pragma once

#include "application/driven_ports/SwitchRepository.h"
#include "driving_adapters/matter/bridged_device_cluster/BridgedDeviceBasicInfoLoader.h"

namespace mobmatter::driving_adapters::matter::switch_cluster {

namespace driven_ports = application::driven_ports;

class SwitchBasicInfoLoader final : public bridged_device_cluster::BridgedDeviceBasicInfoLoader {
public:
    explicit SwitchBasicInfoLoader(driven_ports::SwitchRepository& switchRepository);
    std::optional<bridged_device_cluster::BridgedDeviceBasicInfoData> load(chip::EndpointId endpointId) const override;

private:
    driven_ports::SwitchRepository& mSwitchRepository;
};

}
