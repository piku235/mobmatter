#include "SwitchAttributeAccess.h"
#include "application/model/Switch.h"

#include <app-common/zap-generated/ids/Attributes.h>
#include <app-common/zap-generated/ids/Clusters.h>

#include <cstdint>

using namespace chip;
using namespace chip::app;
using mobmatter::application::driven_ports::SwitchRepository;
using mobmatter::application::model::Switch;

namespace mobmatter::driving_adapters::matter::switch_cluster {

SwitchAttributeAccess::SwitchAttributeAccess(SwitchRepository& switchRepository)
    : AttributeAccessInterface(Optional<EndpointId>::Missing(), Clusters::OnOff::Id)
    , mSwitchRepository(switchRepository)
{
}

CHIP_ERROR SwitchAttributeAccess::Read(const ConcreteReadAttributePath& path, AttributeValueEncoder& encoder)
{
    using namespace Clusters::OnOff::Attributes;

    auto switch_ = mSwitchRepository.find(path.mEndpointId);

    if (!switch_) {
        return CHIP_ERROR_NOT_FOUND;
    }

    switch (path.mAttributeId) {
    case OnOff::Id:
        return encoder.Encode(Switch::State::On == switch_->state());
    default:
        return CHIP_ERROR_INVALID_ARGUMENT;
    }
}

}
