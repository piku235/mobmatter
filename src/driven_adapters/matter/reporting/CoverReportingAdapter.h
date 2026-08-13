#pragma once

#include "application/model/window_covering/CoverEvents.h"
#include "common/domain/MultiDomainEventSubscriber.h"

namespace mobmatter::driven_adapters::matter::reporting {

namespace wc = application::model::window_covering;

class CoverReportingAdapter final : public common::domain::MultiDomainEventSubscriber<wc::CoverLiftCurrentPositionChanged, wc::CoverLiftTargetPositionChanged, wc::CoverLiftMotionChanged, wc::CoverTiltCurrentPositionChanged, wc::CoverTiltTargetPositionChanged, wc::CoverTiltMotionChanged, wc::CoverMarkedAsReachable, wc::CoverMarkedAsUnreachable, wc::CoverRenamed> {
public:
    void handle(const wc::CoverLiftCurrentPositionChanged& event) override;
    void handle(const wc::CoverLiftTargetPositionChanged& event) override;
    void handle(const wc::CoverLiftMotionChanged& event) override;
    void handle(const wc::CoverTiltCurrentPositionChanged& event) override;
    void handle(const wc::CoverTiltTargetPositionChanged& event) override;
    void handle(const wc::CoverTiltMotionChanged& event) override;
    void handle(const wc::CoverMarkedAsReachable& event) override;
    void handle(const wc::CoverMarkedAsUnreachable& event) override;
    void handle(const wc::CoverRenamed& event) override;
};

}
