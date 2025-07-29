#pragma once

#include "application/model/window_covering/CoverAdded.h"
#include "application/model/window_covering/CoverLiftCurrentPositionChanged.h"
#include "application/model/window_covering/CoverLiftTargetPositionChanged.h"
#include "application/model/window_covering/CoverMarkedAsReachable.h"
#include "application/model/window_covering/CoverMarkedAsUnreachable.h"
#include "application/model/window_covering/CoverRemoved.h"
#include "common/domain/MultiDomainEventSubscriber.h"
#include "common/logging/Logger.h"

namespace mmbridge::driven_adapters::logging {

namespace wc = mmbridge::application::model::window_covering;
namespace logging = mmbridge::common::logging;

class LoggingDomainEventSubscriber final : public mmbridge::common::domain::MultiDomainEventSubscriber<wc::CoverAdded, wc::CoverRemoved, wc::CoverLiftTargetPositionChanged, wc::CoverLiftCurrentPositionChanged, wc::CoverMarkedAsReachable, wc::CoverMarkedAsUnreachable> {
public:
    explicit LoggingDomainEventSubscriber(logging::Logger& logger);
    void handle(const wc::CoverAdded& event) override;
    void handle(const wc::CoverRemoved& event) override;
    void handle(const wc::CoverLiftTargetPositionChanged& event) override;
    void handle(const wc::CoverLiftCurrentPositionChanged& event) override;
    void handle(const wc::CoverMarkedAsReachable& event) override;
    void handle(const wc::CoverMarkedAsUnreachable& event) override;

private:
    logging::Logger& mLogger;
};

}
