#pragma once

#include "application/model/MobilusDeviceId.h"
#include "application/model/window_covering/Position.h"

namespace mobmatter::application::driven_ports {

class CoverControlService {
public:
    virtual ~CoverControlService() = default;

    virtual void openCover(model::MobilusDeviceId deviceId) = 0;
    virtual void closeCover(model::MobilusDeviceId deviceId) = 0;
    virtual void liftCover(model::MobilusDeviceId deviceId, model::window_covering::Position position) = 0;
    virtual void tiltCover(model::MobilusDeviceId deviceId, model::window_covering::Position position) = 0;
    virtual void stopCoverMotion(model::MobilusDeviceId deviceId) = 0;
};

}
