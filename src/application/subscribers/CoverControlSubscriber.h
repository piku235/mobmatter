#include "application/driven_ports/CoverControlService.h"
#include "application/model/window_covering/CoverCloseRequested.h"
#include "application/model/window_covering/CoverLiftRequested.h"
#include "application/model/window_covering/CoverOpenRequested.h"
#include "application/model/window_covering/CoverStopMotionRequested.h"
#include "application/model/window_covering/CoverTiltRequested.h"
#include "common/domain/MultiDomainEventSubscriber.h"

namespace mobmatter::application::subscribers {

namespace driven_ports = mobmatter::application::driven_ports;
namespace wc = mobmatter::application::model::window_covering;

class CoverControlSubscriber final : public mobmatter::common::domain::MultiDomainEventSubscriber<wc::CoverOpenRequested, wc::CoverCloseRequested, wc::CoverLiftRequested, wc::CoverTiltRequested, wc::CoverStopMotionRequested> {
public:
    explicit CoverControlSubscriber(driven_ports::CoverControlService& coverControlService);

    void handle(const wc::CoverOpenRequested& event) override;
    void handle(const wc::CoverCloseRequested& event) override;
    void handle(const wc::CoverLiftRequested& event) override;
    void handle(const wc::CoverTiltRequested& event) override;
    void handle(const wc::CoverStopMotionRequested& event) override;

private:
    driven_ports::CoverControlService& mCoverControlService;
};

}
