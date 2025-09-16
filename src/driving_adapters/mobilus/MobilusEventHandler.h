#pragma once

#include "jungi/mobilus_gtw_client/proto/Event.pb.h"

namespace mobmatter::driving_adapters::mobilus {

namespace proto = jungi::mobilus_gtw_client::proto;

class MobilusEventHandler {
public:
    enum class Result {
        UnmatchedDevice,
        Handled,
    };

    virtual ~MobilusEventHandler() = default;
    virtual Result handle(const proto::Event& event) = 0;
};

}
