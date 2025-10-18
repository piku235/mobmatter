#include "application/driven_ports/DeviceManagementService.h"
#include "application/model/window_covering/CoverRenameRequested.h"
#include "common/domain/DomainEventSubscriber.h"

namespace mobmatter::application::subscribers {

namespace driven_ports = mobmatter::application::driven_ports;
namespace wc = mobmatter::application::model::window_covering;

class DeviceManagementSubscriber final : public mobmatter::common::domain::DomainEventSubscriber<wc::CoverRenameRequested> {
public:
    explicit DeviceManagementSubscriber(driven_ports::DeviceManagementService& deviceManagementService);

    void handle(const wc::CoverRenameRequested& event) override;

private:
    driven_ports::DeviceManagementService& mDeviceManagementService;
};

}
