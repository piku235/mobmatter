#include "InMemoryCoverRepository.h"

using namespace mmbridge::application::model;
using namespace mmbridge::application::model::window_covering;

namespace mmbridge::driven_adapters::persistence::in_memory {

void InMemoryCoverRepository::save(const Cover& cover)
{
    mCovers.insert_or_assign(cover.endpointId(), cover);
}

void InMemoryCoverRepository::remove(const Cover& cover)
{
    mCovers.erase(cover.endpointId());
}

std::optional<Cover> InMemoryCoverRepository::findOfMobilusDeviceId(MobilusDeviceId deviceId) const
{
    for (auto& [_, cover] : mCovers) {
        if (deviceId == cover.mobilusDeviceId()) {
            return cover;
        }
    }

    return std::nullopt;
}

std::optional<Cover> InMemoryCoverRepository::find(EndpointId endpointId) const
{
    auto it = mCovers.find(endpointId);

    if (it == mCovers.end()) {
        return std::nullopt;
    }

    return it->second;
}

std::vector<Cover> InMemoryCoverRepository::all() const
{
    std::vector<Cover> covers;
    covers.reserve(mCovers.size());

    for (auto& [_, cover] : mCovers) {
        covers.push_back(cover);
    }

    return covers;
}

}
