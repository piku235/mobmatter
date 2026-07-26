#pragma once

#include "application/driven_ports/DeviceManagementService.h"
#include "application/model/window_covering/CoverRenameRequested.h"
#include "common/domain/DomainEventSubscriber.h"

namespace mobmatter::application::subscribers {

class DeviceManagementSubscriber final : public common::domain::DomainEventSubscriber<model::window_covering::CoverRenameRequested> {
public:
    explicit DeviceManagementSubscriber(driven_ports::DeviceManagementService& deviceManagementService);

    void handle(const model::window_covering::CoverRenameRequested& event) override;

private:
    driven_ports::DeviceManagementService& mDeviceManagementService;
};

}
