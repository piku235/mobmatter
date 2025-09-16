#pragma once

#include "application/model/window_covering/CoverLiftCurrentPositionChanged.h"
#include "application/model/window_covering/CoverLiftTargetPositionChanged.h"
#include "application/model/window_covering/CoverMarkedAsReachable.h"
#include "application/model/window_covering/CoverMarkedAsUnreachable.h"
#include "application/model/window_covering/CoverOperationalStatusChanged.h"
#include "common/domain/MultiDomainEventSubscriber.h"

namespace mobmatter::driven_adapters::matter::reporting {

namespace wc = mobmatter::application::model::window_covering;

class CoverReportingAdapter final : public mobmatter::common::domain::MultiDomainEventSubscriber<wc::CoverLiftCurrentPositionChanged, wc::CoverLiftTargetPositionChanged, wc::CoverMarkedAsReachable, wc::CoverMarkedAsUnreachable, wc::CoverOperationalStatusChanged> {
public:
    void handle(const wc::CoverLiftCurrentPositionChanged& event) override;
    void handle(const wc::CoverLiftTargetPositionChanged& event) override;
    void handle(const wc::CoverMarkedAsReachable& event) override;
    void handle(const wc::CoverMarkedAsUnreachable& event) override;
    void handle(const wc::CoverOperationalStatusChanged& event) override;
};

}
