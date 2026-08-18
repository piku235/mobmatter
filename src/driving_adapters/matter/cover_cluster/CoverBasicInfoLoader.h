#pragma once

#include "application/driven_ports/CoverRepository.h"
#include "driving_adapters/matter/bridged_device_cluster/BridgedDeviceBasicInfoLoader.h"

namespace mobmatter::driving_adapters::matter::cover_cluster {

namespace driven_ports = application::driven_ports;

class CoverBasicInfoLoader final : public bridged_device_cluster::BridgedDeviceBasicInfoLoader {
public:
    explicit CoverBasicInfoLoader(driven_ports::CoverRepository& coverRepository);
    std::optional<bridged_device_cluster::BridgedDeviceBasicInfoData> load(chip::EndpointId endpointId) const override;

private:
    driven_ports::CoverRepository& mCoverRepository;
};

}
