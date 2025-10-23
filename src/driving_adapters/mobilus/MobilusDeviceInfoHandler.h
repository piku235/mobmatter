#pragma once

#include "HandlerResult.h"
#include "jungi/mobilus_gtw_client/proto/Device.pb.h"

namespace mobmatter::driving_adapters::mobilus {

namespace proto = jungi::mobilus_gtw_client::proto;

class MobilusDeviceInfoHandler {
public:
    virtual ~MobilusDeviceInfoHandler() = default;
    virtual HandlerResult handle(const proto::Device& deviceInfo) = 0;
};

}
