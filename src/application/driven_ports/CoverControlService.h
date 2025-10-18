#pragma once

#include "application/model/MobilusDeviceId.h"
#include "application/model/window_covering/Position.h"

namespace mobmatter::application::driven_ports {

namespace model = mobmatter::application::model;

class CoverControlService {
public:
    virtual ~CoverControlService() = default;

    virtual void liftCover(model::MobilusDeviceId deviceId, model::window_covering::Position position) = 0;
    virtual void stopCoverMotion(model::MobilusDeviceId deviceId) = 0;
};

}
