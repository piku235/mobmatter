#pragma once

#include "application/driven_ports/SwitchControlService.h"
#include "application/model/SwitchEvents.h"
#include "common/domain/MultiDomainEventSubscriber.h"

namespace mobmatter::application::subscribers {

class SwitchControlSubscriber final : public common::domain::MultiDomainEventSubscriber<model::SwitchTurnOnRequested, model::SwitchTurnOffRequested> {
public:
    explicit SwitchControlSubscriber(driven_ports::SwitchControlService& switchControlService);

    void handle(const model::SwitchTurnOnRequested& event);
    void handle(const model::SwitchTurnOffRequested& event);

private:
    driven_ports::SwitchControlService& mSwitchControlService;
};

}
