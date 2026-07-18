#include "MobilusCoverHandler.h"
#include "MobilusCoverPositionState.h"
#include "application/model/window_covering/Cover.h"
#include "application/model/window_covering/CoverSpecification.h"
#include "application/model/window_covering/PositionState.h"
#include "driving_adapters/mobilus/Log.h"

#include <jungi/mobgtw/EventNumber.h>

#include <cinttypes>

#define LOG_SUFFIX " [md=%" PRId64 "]"
#define LOG_SUFFIX_EP " [ep=%u, md=%" PRId64 "]"

using namespace jungi::mobgtw;
using namespace mobmatter::application::model;
using namespace mobmatter::application::model::window_covering;

namespace mobmatter::driving_adapters::mobilus::device_handlers {

MobilusCoverHandler::MobilusCoverHandler(driven_ports::CoverRepository& coverRepository, driven_ports::EndpointIdGenerator& endpointIdGenerator, logging::Logger& logger)
    : mCoverRepository(coverRepository)
    , mEndpointIdGenerator(endpointIdGenerator)
    , mLogger(logger)
{
}

void MobilusCoverHandler::sync(const DeviceStateMap& devices)
{
    // non-existing
    for (auto& cover : mCoverRepository.all()) {
        if (devices.end() == devices.find(cover.mobilusDeviceId())) {
            cover.reportRemoved();
            mCoverRepository.remove(cover);

            mLogger.notice(LOG_TAG "Removed cover" LOG_SUFFIX_EP, cover.endpointId(), cover.mobilusDeviceId());
        }
    }

    // existing
    for (auto& [deviceId, deviceState] : devices) {
        auto coverSpec = CoverSpecification::findFor(static_cast<MobilusDeviceType>(deviceState.device.type()));

        if (!coverSpec) {
            continue;
        }

        auto cover = mCoverRepository.findOfMobilusDeviceId(deviceId);

        if (cover) {
            if (apply(*cover, deviceState.device) | apply(*cover, deviceState.lastEvent)) {
                mCoverRepository.save(*cover);
            }

            continue;
        }

        init(std::move(*coverSpec), deviceState.device, deviceState.lastEvent);
    }
}

MobilusDeviceEventHandler::Result MobilusCoverHandler::handle(const proto::Event& event)
{
    if (!event.has_device_id()) {
        return Result::Unmatched;
    }

    auto cover = mCoverRepository.findOfMobilusDeviceId(event.device_id());

    if (!cover) {
        return Result::Unmatched;
    }

    if (apply(*cover, event)) {
        mCoverRepository.save(*cover);
    }

    return Result::Handled;
}

void MobilusCoverHandler::init(CoverSpecification coverSpec, const proto::Device& deviceInfo, const proto::Event& lastEvent)
{
    auto liftState = coverSpec.featureFlags().has(CoverFeature::Lift)
        ? PositionState::at(Position::fullyClosed())
        : PositionState::unavailable();
    auto tiltState = coverSpec.featureFlags().has(CoverFeature::Tilt)
        ? PositionState::at(Position::fullyClosed())
        : PositionState::unavailable();

    if (EventNumber::Reached == lastEvent.event_number()) {
        auto result = MobilusCoverPositionState::parse(lastEvent.value());

        if (!result.isValid()) {
            mLogger.error(LOG_TAG "Invalid cover position: %s" LOG_SUFFIX, lastEvent.value().c_str(), deviceInfo.id());
        }

        if (result.liftPosition) {
            liftState = PositionState::at(*result.liftPosition);
        }
        if (result.tiltPosition) {
            tiltState = PositionState::at(*result.tiltPosition);
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
        std::move(coverSpec),
        deviceInfo.name(),
        std::move(liftState),
        std::move(tiltState));
    mCoverRepository.save(cover);

    mLogger.notice(LOG_TAG "Added cover" LOG_SUFFIX_EP, cover.endpointId(), deviceInfo.id());
}

bool MobilusCoverHandler::apply(Cover& cover, const proto::Device& deviceInfo)
{
    if (Cover::Result::Ok == cover.reportRenamedTo(deviceInfo.name())) {
        mLogger.notice(LOG_TAG "Renamed cover to: %s" LOG_SUFFIX_EP, deviceInfo.name().c_str(), cover.endpointId(), cover.mobilusDeviceId());
        return true;
    }

    return false;
}

bool MobilusCoverHandler::apply(Cover& cover, const proto::Event& event)
{
    switch (event.event_number()) {
    case EventNumber::Sent: {
        if ("UP" == event.value()) {
            if (Cover::Result::Ok == cover.reportOpen()) {
                mLogger.notice(LOG_TAG "Started opening cover" LOG_SUFFIX_EP, cover.endpointId(), cover.mobilusDeviceId());
                return true;
            }

            return false;
        }

        if ("DOWN" == event.value()) {
            if (Cover::Result::Ok == cover.reportClose()) {
                mLogger.notice(LOG_TAG "Started closing cover" LOG_SUFFIX_EP, cover.endpointId(), cover.mobilusDeviceId());
                return true;
            }

            return false;
        }

        if ("STOP" == event.value()) {
            if (Cover::Result::Ok == cover.reportStopMotion()) {
                mLogger.notice(LOG_TAG "Stopping cover motion" LOG_SUFFIX_EP, cover.endpointId(), cover.mobilusDeviceId());
                return true;
            }

            return false;
        }

        auto positionState = MobilusCoverPositionState::parse(event.value());

        if (!positionState.isValid()) {
            mLogger.error(LOG_TAG "Invalid cover position: %s" LOG_SUFFIX_EP, event.value().c_str(), cover.endpointId(), cover.mobilusDeviceId());
            return false;
        }

        bool result = false;

        if (positionState.liftPosition && Cover::Result::Ok == cover.reportLiftTo(*positionState.liftPosition)) {
            mLogger.notice(LOG_TAG "Started cover lift to target position: %d%%" LOG_SUFFIX_EP, positionState.liftPosition->closedPercent().value(), cover.endpointId(), cover.mobilusDeviceId());
            result = true;
        }

        if (positionState.tiltPosition && Cover::Result::Ok == cover.reportTiltTo(*positionState.tiltPosition)) {
            mLogger.notice(LOG_TAG "Started cover tilt to target position: %d%%" LOG_SUFFIX_EP, positionState.tiltPosition->closedPercent().value(), cover.endpointId(), cover.mobilusDeviceId());
            result = true;
        }

        return result;
    }
    case EventNumber::Reached: {
        auto positionState = MobilusCoverPositionState::parse(event.value());

        if (!positionState.isValid()) {
            mLogger.error(LOG_TAG "Invalid cover position: %s" LOG_SUFFIX_EP, event.value().c_str(), cover.endpointId(), cover.mobilusDeviceId());
            return false;
        }

        bool result = false;

        if (Cover::Result::Ok == cover.reportReachable()) {
            mLogger.notice(LOG_TAG "Cover marked as reachable" LOG_SUFFIX_EP, cover.endpointId(), cover.mobilusDeviceId());
            result = true;
        }

        if (positionState.liftPosition && Cover::Result::Ok == cover.reportLiftPosition(*positionState.liftPosition)) {
            mLogger.notice(LOG_TAG "Changed cover lift position: %d%%" LOG_SUFFIX_EP, positionState.liftPosition->closedPercent().value(), cover.endpointId(), cover.mobilusDeviceId());
            result = true;
        }

        if (positionState.tiltPosition && Cover::Result::Ok == cover.reportTiltPosition(*positionState.tiltPosition)) {
            mLogger.notice(LOG_TAG "Changed cover tilt position: %d%%" LOG_SUFFIX_EP, positionState.tiltPosition->closedPercent().value(), cover.endpointId(), cover.mobilusDeviceId());
            result = true;
        }

        return result;
    }
    case EventNumber::Error: {
        bool result = false;

        if ("NO_CONNECTION" == event.value() && Cover::Result::Ok == cover.reportUnreachable()) {
            mLogger.notice(LOG_TAG "Cover marked as unreachable" LOG_SUFFIX_EP, cover.endpointId(), cover.mobilusDeviceId());
            result = true;
        }

        if (Cover::Result::Ok == cover.reportMotionFailure()) {
            mLogger.notice(LOG_TAG "Cover motion failed: %s" LOG_SUFFIX_EP, event.value().c_str(), cover.endpointId(), cover.mobilusDeviceId());
            result = true;
        }

        return result;
    }
    default:
        mLogger.notice(LOG_TAG "Unknown event number");
        return false;
    }
}

}