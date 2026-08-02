#pragma once

#include "application/model/SwitchEvents.h"
#include "common/domain/MultiDomainEventSubscriber.h"

namespace mobmatter::driven_adapters::matter::reporting {

namespace model = application::model;

class SwitchReportingAdapter final : public common::domain::MultiDomainEventSubscriber<model::SwitchTurnedOn, model::SwitchTurnedOff, model::SwitchMarkedAsReachable, model::SwitchMarkedAsUnreachable, model::SwitchNameChanged> {
public:
    void handle(const model::SwitchTurnedOn& event) override;
    void handle(const model::SwitchTurnedOff& event) override;
    void handle(const model::SwitchMarkedAsReachable& event) override;
    void handle(const model::SwitchMarkedAsUnreachable& event) override;
    void handle(const model::SwitchNameChanged& event) override;
};

}
