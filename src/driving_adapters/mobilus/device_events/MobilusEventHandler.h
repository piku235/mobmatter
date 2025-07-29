#pragma once

#include "jungi/mobilus_gtw_client/proto/Event.pb.h"

namespace mmbridge::driving_adapters::mobilus::device_events {

namespace proto = jungi::mobilus_gtw_client::proto;

class MobilusEventHandler {
public:
    enum class Result { UnmatchedDevice,
        Handled };

    virtual ~MobilusEventHandler() = default;
    virtual Result handle(const proto::Event& event) = 0;
};

}
