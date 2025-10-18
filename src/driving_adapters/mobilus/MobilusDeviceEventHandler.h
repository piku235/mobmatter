#pragma once

#include "jungi/mobilus_gtw_client/proto/Event.pb.h"

namespace mobmatter::driving_adapters::mobilus {

namespace proto = jungi::mobilus_gtw_client::proto;

class MobilusDeviceEventHandler {
public:
    enum class Result {
        Unsupported,
        Handled,
    };

    virtual ~MobilusDeviceEventHandler() = default;
    virtual Result handle(const proto::Event& event) = 0;
};

}
