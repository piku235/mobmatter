#pragma once

#include "HandlerResult.h"
#include "jungi/mobilus_gtw_client/proto/Event.pb.h"

namespace mobmatter::driving_adapters::mobilus {

namespace proto = jungi::mobilus_gtw_client::proto;

class MobilusDeviceEventHandler {
public:
    virtual ~MobilusDeviceEventHandler() = default;
    virtual HandlerResult handle(const proto::Event& event) = 0;
};

}
