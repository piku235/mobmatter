#pragma once

#include "application/model/MobilusDeviceId.h"
#include "application/model/window_covering/Position.h"

namespace mobmatter::application::driven_ports {

namespace model = mobmatter::application::model;

class MobilusCoverControlService {
public:
    virtual ~MobilusCoverControlService() = default;
    virtual void liftCover(model::MobilusDeviceId mobilusDeviceId, model::window_covering::Position position) = 0;
    virtual void stopCoverMotion(model::MobilusDeviceId mobilusDeviceId) = 0;
};

}
