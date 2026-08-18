#include "CoverBasicInfoLoader.h"

#include <string>

using namespace mobmatter::driving_adapters::matter::bridged_device_cluster;
using mobmatter::application::driven_ports::CoverRepository;

namespace mobmatter::driving_adapters::matter::cover_cluster {

CoverBasicInfoLoader::CoverBasicInfoLoader(CoverRepository& coverRepository)
    : mCoverRepository(coverRepository)
{
}

std::optional<BridgedDeviceBasicInfoData> CoverBasicInfoLoader::load(chip::EndpointId endpointId) const
{
    auto cover = mCoverRepository.find(endpointId);
    if (!cover) {
        return std::nullopt;
    }

    return BridgedDeviceBasicInfoData {
        cover->endpointId(),
        std::to_string(cover->mobilusDeviceId()),
        cover->isReachable(),
        cover->specification().model(),
        cover->name(),
    };
}

}
