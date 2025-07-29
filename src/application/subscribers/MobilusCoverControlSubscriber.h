#include "application/driven_ports/MobilusCoverControlService.h"
#include "application/model/window_covering/CoverLiftRequested.h"
#include "application/model/window_covering/CoverStopMotionRequested.h"
#include "common/domain/MultiDomainEventSubscriber.h"

namespace mmbridge::application::subscribers {

namespace driven_ports = mmbridge::application::driven_ports;
namespace wc = mmbridge::application::model::window_covering;

class MobilusCoverControlSubscriber final : public mmbridge::common::domain::MultiDomainEventSubscriber<wc::CoverLiftRequested, wc::CoverStopMotionRequested> {
public:
    MobilusCoverControlSubscriber(driven_ports::MobilusCoverControlService& coverControlService);
    void handle(const wc::CoverLiftRequested& event) override;
    void handle(const wc::CoverStopMotionRequested& event) override;

private:
    driven_ports::MobilusCoverControlService& mCoverControlService;
};

}
