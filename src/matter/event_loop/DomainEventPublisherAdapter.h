#pragma once

#include "matter/AppComponent.h"
#include <system/SystemLayer.h>

namespace mmbridge::matter::event_loop {

class DomainEventPublisherAdapter final : public mmbridge::matter::AppComponent,
                                          public chip::System::EventLoopHandler {
public:
    DomainEventPublisherAdapter(chip::System::LayerSocketsLoop& systemLayer);
    ~DomainEventPublisherAdapter();

    void boot() override;
    void shutdown() override;

    void HandleEvents() override;

private:
    bool mRegistered = false;
    chip::System::LayerSocketsLoop& mSystemLayer;
};

}
