#pragma once

#include "matter/AppComponent.h"
#include <system/SystemLayer.h>

namespace mobmatter::matter::event_loop {

class DomainEventPublisherAdapter final : public mobmatter::matter::AppComponent,
                                          public chip::System::EventLoopHandler {
public:
    DomainEventPublisherAdapter(chip::System::LayerSocketsLoop& systemLayer);

    void boot() override;
    void shutdown() override;

    // EventLoopHandler
    void HandleEvents() override;

private:
    chip::System::LayerSocketsLoop& mSystemLayer;
};

}
