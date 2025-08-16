#include "MobilusCoverInitHandler.h"
#include "application/model/MobilusDeviceId.h"
#include "application/model/window_covering/CoverSpecification.h"
#include "application/model/window_covering/PositionState.h"
#include "driving_adapters/mobilus/ConversionUtils.h"
#include "driving_adapters/mobilus/Log.h"
#include "jungi/mobilus_gtw_client/EventNumber.h"

#include <cinttypes>

using namespace jungi::mobilus_gtw_client;
using namespace mmbridge::application::model;
using namespace mmbridge::application::model::window_covering;

namespace mmbridge::driving_adapters::mobilus::cover {

MobilusCoverInitHandler::MobilusCoverInitHandler(driven_ports::CoverRepository& coverRepository, driven_ports::EndpointIdGenerator& endpointIdGenerator, logging::Logger& logger)
    : mCoverRepository(coverRepository)
    , mEndpointIdGenerator(endpointIdGenerator)
    , mLogger(logger)
{
}

void MobilusCoverInitHandler::initDevice(const proto::Device& device, const proto::Event& currentState)
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
            mLogger.error(LOG_TAG "Invalid cover lift position: %s [md=%" PRId64 "]", currentState.value().c_str(), device.id());
        }
    }

    auto cover = Cover::add(
        mEndpointIdGenerator.next(),
        static_cast<MobilusDeviceId>(device.id()),
        device.name(),
        PositionState::at(liftPosition.value_or(Position::fullyClosed())),
        std::move(*coverSpec));
    mCoverRepository.save(cover);
}

bool MobilusCoverInitHandler::supports(MobilusDeviceType deviceType)
{
    return CoverSpecification::findFor(deviceType).has_value();
}

}
