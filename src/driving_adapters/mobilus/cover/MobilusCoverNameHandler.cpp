#include "MobilusCoverNameHandler.h"
#include "application/model/window_covering/Cover.h"
#include "application/model/window_covering/CoverSpecification.h"
#include "driving_adapters/mobilus/Log.h"

using namespace mobmatter::application::model::window_covering;
using mobmatter::application::model::window_covering::Cover;

namespace mobmatter::driving_adapters::mobilus::cover {

MobilusCoverNameHandler::MobilusCoverNameHandler(driven_ports::CoverRepository& coverRepository, logging::Logger& logger)
    : mCoverRepository(coverRepository)
    , mLogger(logger)
{
}

void MobilusCoverNameHandler::handle(model::MobilusDeviceId deviceId, const std::string& name)
{
    auto cover = mCoverRepository.findOfMobilusDeviceId(deviceId);

    if (cover && Cover::Result::Ok == cover->rename(name)) {
        mCoverRepository.save(*cover);
        mLogger.notice(LOG_TAG "Renamed cover to: %s" LOG_SUFFIX, name.c_str(), cover->endpointId(), cover->mobilusDeviceId());
    }
}

bool MobilusCoverNameHandler::supports(model::MobilusDeviceType deviceType) const
{
    return CoverSpecification::findFor(deviceType).has_value();
}

}
