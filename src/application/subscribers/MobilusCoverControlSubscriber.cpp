#include "MobilusCoverControlSubscriber.h"

using namespace mmbridge::application::model::window_covering;
using mmbridge::application::driven_ports::MobilusCoverControlService;

namespace mmbridge::application::subscribers {

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
