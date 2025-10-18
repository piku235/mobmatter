#include "application/driven_ports/CoverEndpointService.h"
#include "application/model/window_covering/CoverAdded.h"
#include "application/model/window_covering/CoverRemoved.h"
#include "common/domain/MultiDomainEventSubscriber.h"

namespace mobmatter::application::subscribers {

namespace driven_ports = mobmatter::application::driven_ports;
namespace wc = mobmatter::application::model::window_covering;

class CoverEndpointSubscriber final : public mobmatter::common::domain::MultiDomainEventSubscriber<wc::CoverAdded, wc::CoverRemoved> {
public:
    explicit CoverEndpointSubscriber(driven_ports::CoverEndpointService& coverEndpointService);
    
    void handle(const wc::CoverAdded& event) override;
    void handle(const wc::CoverRemoved& event) override;

private:
    driven_ports::CoverEndpointService& mCoverEndpointService;
};

}
