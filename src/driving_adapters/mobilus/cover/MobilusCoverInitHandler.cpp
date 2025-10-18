#include "MobilusCoverInitHandler.h"
#include "application/model/MobilusDeviceId.h"
#include "application/model/window_covering/CoverSpecification.h"
#include "application/model/window_covering/PositionState.h"
#include "driving_adapters/mobilus/ConversionUtils.h"
#include "driving_adapters/mobilus/Log.h"
#include "jungi/mobilus_gtw_client/EventNumber.h"

#include <cinttypes>

#define LOG_SUFFIX " [md=%" PRId64 "]"

using namespace jungi::mobilus_gtw_client;
using namespace mobmatter::application::model;
using namespace mobmatter::application::model::window_covering;

namespace mobmatter::driving_adapters::mobilus::cover {

MobilusCoverInitHandler::MobilusCoverInitHandler(driven_ports::CoverRepository& coverRepository, driven_ports::EndpointIdGenerator& endpointIdGenerator, logging::Logger& logger)
    : mCoverRepository(coverRepository)
    , mEndpointIdGenerator(endpointIdGenerator)
    , mLogger(logger)
{
}

void MobilusCoverInitHandler::handle(const proto::Device& device, const proto::Event& currentState)
{
    auto coverSpec = CoverSpecification::findFor(static_cast<MobilusDeviceType>(device.type()));

    if (!coverSpec) {
        // should not occur
        return;
    }

    std::optional<Position> liftPosition;

    if (EventNumber::Sent == currentState.event_number() || EventNumber::Reached == currentState.event_number()) {
        liftPosition = ConversionUtils::convertLiftPosition(currentState.value());

        if (!liftPosition) {
            mLogger.error(LOG_TAG "Invalid cover lift position: %s" LOG_SUFFIX, currentState.value().c_str(), device.id());
        }
    }

    auto endpointId = mEndpointIdGenerator.next();

    if (!endpointId) {
        mLogger.error(LOG_TAG "Could not get next endpoint id" LOG_SUFFIX, device.id());
    }

    auto cover = Cover::add(
        *endpointId,
        static_cast<MobilusDeviceId>(device.id()),
        device.name(),
        PositionState::at(liftPosition.value_or(Position::fullyClosed())),
        std::move(*coverSpec));
    mCoverRepository.save(cover);

    mLogger.notice(LOG_TAG "Added cover" LOG_SUFFIX, cover.endpointId(), device.id());
}

bool MobilusCoverInitHandler::supports(MobilusDeviceType deviceType) const
{
    return CoverSpecification::findFor(deviceType).has_value();
}

}
