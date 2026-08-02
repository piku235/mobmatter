#pragma once

#include "application/driven_ports/SwitchEndpointService.h"
#include "application/model/SwitchEvents.h"
#include "common/domain/MultiDomainEventSubscriber.h"

namespace mobmatter::application::subscribers {

class SwitchEndpointSubscriber final : public common::domain::MultiDomainEventSubscriber<model::SwitchAdded, model::SwitchRemoved> {
public:
    explicit SwitchEndpointSubscriber(driven_ports::SwitchEndpointService& switchEndpointService);

    void handle(const model::SwitchAdded& event) override;
    void handle(const model::SwitchRemoved& event) override;

private:
    driven_ports::SwitchEndpointService& mSwitchEndpointService;
};

}
