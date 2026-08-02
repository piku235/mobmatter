#pragma once

#include "application/driven_ports/CoverEndpointService.h"
#include "application/model/window_covering/CoverEvents.h"
#include "common/domain/MultiDomainEventSubscriber.h"

namespace mobmatter::application::subscribers {

class CoverEndpointSubscriber final : public common::domain::MultiDomainEventSubscriber<model::window_covering::CoverAdded, model::window_covering::CoverRemoved> {
public:
    explicit CoverEndpointSubscriber(driven_ports::CoverEndpointService& coverEndpointService);

    void handle(const model::window_covering::CoverAdded& event) override;
    void handle(const model::window_covering::CoverRemoved& event) override;

private:
    driven_ports::CoverEndpointService& mCoverEndpointService;
};

}
