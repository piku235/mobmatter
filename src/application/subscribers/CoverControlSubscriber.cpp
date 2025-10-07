#include "CoverControlSubscriber.h"

using namespace mobmatter::application::model::window_covering;
using mobmatter::application::driven_ports::CoverControlService;

namespace mobmatter::application::subscribers {

CoverControlSubscriber::CoverControlSubscriber(CoverControlService& coverControlService)
    : mCoverControlService(coverControlService)
{
}

void CoverControlSubscriber::handle(const CoverLiftRequested& event)
{
    mCoverControlService.liftCover(event.mobilusDeviceId, event.position);
}

void CoverControlSubscriber::handle(const CoverStopMotionRequested& event)
{
    mCoverControlService.stopCoverMotion(event.mobilusDeviceId);
}

}
