#pragma once

#include "HandlerResult.h"
#include "jungi/mobilus_gtw_client/proto/Device.pb.h"
#include "jungi/mobilus_gtw_client/proto/Event.pb.h"

namespace mobmatter::driving_adapters::mobilus {

namespace proto = jungi::mobilus_gtw_client::proto;

class MobilusDeviceSyncHandler {
public:
    virtual ~MobilusDeviceSyncHandler() = default;
    virtual HandlerResult handle(const proto::Device& deviceInfo, const proto::Event& lastEvent) = 0;
};

}
