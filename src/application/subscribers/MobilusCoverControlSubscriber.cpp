#include "MobilusCoverControlSubscriber.h"

using namespace mobmatter::application::model::window_covering;
using mobmatter::application::driven_ports::MobilusCoverControlService;

namespace mobmatter::application::subscribers {

MobilusCoverControlSubscriber::MobilusCoverControlSubscriber(MobilusCoverControlService& coverControlService)
    : mCoverControlService(coverControlService)
{
}

void MobilusCoverControlSubscriber::handle(const CoverLiftRequested& event)
{
    mCoverControlService.liftCover(event.mobilusDeviceId, event.position);
}

void MobilusCoverControlSubscriber::handle(const CoverStopMotionRequested& event)
{
    mCoverControlService.stopCoverMotion(event.mobilusDeviceId);
}

}
