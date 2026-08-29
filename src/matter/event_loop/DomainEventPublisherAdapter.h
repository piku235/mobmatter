#pragma once

#include "matter/AppComponent.h"
#include <system/SystemLayer.h>

namespace mobmatter::matter::event_loop {

class DomainEventPublisherAdapter final : public chip::System::EventLoopHandler,
                                          public AppComponent {
public:
    explicit DomainEventPublisherAdapter(chip::System::LayerSelectLoop& selectLoop);

    void boot() override;
    void shutdown() override;

    // EventLoopHandler
    void HandleEvents() override;

private:
    chip::System::LayerSelectLoop& mSelectLoop;
};

}
