#pragma once

#include "application/model/MobilusDeviceType.h"
#include "jungi/mobilus_gtw_client/proto/Device.pb.h"
#include "jungi/mobilus_gtw_client/proto/Event.pb.h"

namespace mobmatter::driving_adapters::mobilus {

namespace model = mobmatter::application::model;
namespace proto = jungi::mobilus_gtw_client::proto;

class MobilusDeviceInitHandler {
public:
    virtual ~MobilusDeviceInitHandler() = default;

    virtual void initDevice(const proto::Device& device, const proto::Event& currentState) = 0;
    virtual bool supports(model::MobilusDeviceType deviceType) = 0;
};

}
