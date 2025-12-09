#pragma once

#include "jungi/mobgtw/proto/Event.pb.h"

namespace mobmatter::driving_adapters::mobilus {

namespace proto = jungi::mobgtw::proto;

class MobilusDeviceEventHandler {
public:
    enum class Result {
        Unmatched,
        Handled,
    };

    virtual ~MobilusDeviceEventHandler() = default;
    virtual Result handle(const proto::Event& event) = 0;
};

}
