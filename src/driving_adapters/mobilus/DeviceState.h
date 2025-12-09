#pragma once

#include "jungi/mobgtw/proto/Device.pb.h"
#include "jungi/mobgtw/proto/Event.pb.h"

namespace mobmatter::driving_adapters::mobilus {

namespace proto = jungi::mobgtw::proto;

struct DeviceState final {
    proto::Device device;
    proto::Event lastEvent;
};

}
