#include "MobilusCoverHandler.h"
#include "application/model/MobilusDeviceId.h"
#include "application/model/Percent.h"
#include "application/model/window_covering/Cover.h"
#include "application/model/window_covering/CoverSpecification.h"
#include "application/model/window_covering/PositionState.h"
#include "driving_adapters/mobilus/Log.h"
#include "jungi/mobilus_gtw_client/EventNumber.h"

#include <cinttypes>

#define LOG_SUFFIX " [md=%" PRId64 "]"
#define LOG_SUFFIX_EP " [ep=%u, md=%" PRId64 "]"

using namespace jungi::mobilus_gtw_client;
using namespace mobmatter::application::model;
using namespace mobmatter::application::model::window_covering;

namespace mobmatter::driving_adapters::mobilus::device_handlers {

MobilusCoverHandler::MobilusCoverHandler(driven_ports::CoverRepository& coverRepository, driven_ports::EndpointIdGenerator& endpointIdGenerator, logging::Logger& logger)
    : mCoverRepository(coverRepository)
    , mEndpointIdGenerator(endpointIdGenerator)
    , mLogger(logger)
{
}

HandlerResult MobilusCoverHandler::handle(const proto::Device& deviceInfo, const proto::Event& lastEvent)
{
    auto cover = mCoverRepository.findOfMobilusDeviceId(deviceInfo.id());

    if (cover) {
        if (apply(*cover, deviceInfo) | apply(*cover, lastEvent)) {
            mCoverRepository.save(*cover);
        }

        return HandlerResult::Handled;
    }

    auto coverSpec = CoverSpecification::findFor(static_cast<MobilusDeviceType>(deviceInfo.type()));

    if (!coverSpec) {
        return HandlerResult::Unmatched;
    }

    init(std::move(*coverSpec), deviceInfo, lastEvent);

    return HandlerResult::Handled;
}

HandlerResult MobilusCoverHandler::handle(const proto::Event& event)
{
    if (!event.has_device_id()) {
        return HandlerResult::Unmatched;
    }

    auto cover = mCoverRepository.findOfMobilusDeviceId(event.device_id());

    if (!cover) {
        return HandlerResult::Unmatched;
    }

    if (apply(*cover, event)) {
        mCoverRepository.save(*cover);
    }

    return HandlerResult::Handled;
}

void MobilusCoverHandler::init(CoverSpecification coverSpec, const proto::Device& deviceInfo, const proto::Event& lastEvent)
{
    std::optional<Position> liftPosition;

    if (EventNumber::Sent == lastEvent.event_number() || EventNumber::Reached == lastEvent.event_number()) {
        liftPosition = convertLiftPosition(lastEvent.value());

        if (!liftPosition) {
            mLogger.error(LOG_TAG "Invalid cover lift position: %s" LOG_SUFFIX, lastEvent.value().c_str(), deviceInfo.id());
        }
    }

    auto endpointId = mEndpointIdGenerator.next();

    if (!endpointId) {
        mLogger.error(LOG_TAG "Could not get next endpoint id" LOG_SUFFIX, deviceInfo.id());
        return;
    }

    auto cover = Cover::add(
        *endpointId,
        deviceInfo.id(),
        deviceInfo.name(),
        PositionState::at(liftPosition.value_or(Position::fullyClosed())),
        std::move(coverSpec));
    mCoverRepository.save(cover);

    mLogger.notice(LOG_TAG "Added cover" LOG_SUFFIX_EP, cover.endpointId(), deviceInfo.id());
}

bool MobilusCoverHandler::apply(Cover& cover, const proto::Device& deviceInfo)
{
    if (Cover::Result::Ok == cover.rename(deviceInfo.name())) {
        mLogger.notice(LOG_TAG "Renamed cover to: %s" LOG_SUFFIX, deviceInfo.name().c_str(), cover.endpointId(), cover.mobilusDeviceId());
        return true;
    }

    return false;
}

bool MobilusCoverHandler::apply(Cover& cover, const proto::Event& event)
{
    switch (event.event_number()) {
    case EventNumber::Sent: {
        auto position = convertLiftPosition(event.value());

        if (!position) {
            if ("STOP" == event.value()) {
                cover.initiateStopMotion();
                return true;
            }

            mLogger.error(LOG_TAG "Invalid cover lift position: %s" LOG_SUFFIX, event.value().c_str(), cover.endpointId(), cover.mobilusDeviceId());
            return false;
        }

        if (Cover::Result::Ok == cover.startLiftTo(*position)) {
            mLogger.notice(LOG_TAG "Started lifting cover to target position: %d%%" LOG_SUFFIX, position->closedPercent().value(), cover.endpointId(), cover.mobilusDeviceId());
            return true;
        }

        return false;
    }
    case EventNumber::Reached: {
        auto position = convertLiftPosition(event.value());

        if (!position) {
            mLogger.error(LOG_TAG "Invalid cover lift position: %s" LOG_SUFFIX, event.value().c_str(), cover.endpointId(), cover.mobilusDeviceId());
            return false;
        }

        if (Cover::Result::Ok == cover.changeLiftPosition(*position)) {
            mLogger.notice(LOG_TAG "Changed cover lift position: %d%%" LOG_SUFFIX, position->closedPercent().value(), cover.endpointId(), cover.mobilusDeviceId());
            return true;
        }

        return false;
    }
    case EventNumber::Error:
        if ("NO_CONNECTION" == event.value()) {
            if (Cover::Result::Ok == cover.markAsUnreachable()) {
                mLogger.notice(LOG_TAG "Cover marked as unreachable" LOG_SUFFIX, cover.endpointId(), cover.mobilusDeviceId());
                return true;
            }

            return false;
        }

        if (Cover::Result::Ok == cover.failMotion()) {
            mLogger.notice(LOG_TAG "Cover motion failed: %s" LOG_SUFFIX, event.value().c_str(), cover.endpointId(), cover.mobilusDeviceId());
            return true;
        }

        return false;
    default:
        mLogger.notice(LOG_TAG "Unknown event number");
        return false;
    }
}

std::optional<Position> MobilusCoverHandler::convertLiftPosition(const std::string& value)
{
    uint8_t parsedPercent;

    if ('%' == value.back() && 1 == sscanf(value.c_str(), "%hhu", &parsedPercent)) {
        if (auto percent = Percent::from(parsedPercent)) {
            return Position::open(*percent);
        }

        return std::nullopt;
    }

    if ("UP" == value) {
        return Position::fullyOpen();
    }

    if ("DOWN" == value) {
        return Position::fullyClosed();
    }

    return std::nullopt;
}

}
