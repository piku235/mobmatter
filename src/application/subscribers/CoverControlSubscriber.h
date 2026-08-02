#pragma once

#include "application/driven_ports/CoverControlService.h"
#include "application/model/window_covering/CoverEvents.h"
#include "common/domain/MultiDomainEventSubscriber.h"

namespace mobmatter::application::subscribers {

class CoverControlSubscriber final : public common::domain::MultiDomainEventSubscriber<model::window_covering::CoverOpenRequested, model::window_covering::CoverCloseRequested, model::window_covering::CoverLiftRequested, model::window_covering::CoverTiltRequested, model::window_covering::CoverStopMotionRequested> {
public:
    explicit CoverControlSubscriber(driven_ports::CoverControlService& coverControlService);

    void handle(const model::window_covering::CoverOpenRequested& event) override;
    void handle(const model::window_covering::CoverCloseRequested& event) override;
    void handle(const model::window_covering::CoverLiftRequested& event) override;
    void handle(const model::window_covering::CoverTiltRequested& event) override;
    void handle(const model::window_covering::CoverStopMotionRequested& event) override;

private:
    driven_ports::CoverControlService& mCoverControlService;
};

}
