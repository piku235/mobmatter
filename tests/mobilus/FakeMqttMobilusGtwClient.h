#pragma once

#include "jungi/mobilus_gtw_client/MqttMobilusGtwClient.h"

#include <memory>
#include <vector>

namespace mmbridge::tests::mobilus {

namespace mobgtw = jungi::mobilus_gtw_client;

class FakeMqttMobilusGtwClient final : public mobgtw::MqttMobilusGtwClient {
public:
    std::vector<std::unique_ptr<google::protobuf::MessageLite>> sentMessages;

    Result<> connect() override
    {
        return {};
    }

    Result<> disconnect() override
    {
        return {};
    }

    Result<> send(const google::protobuf::MessageLite& message) override
    {
        sentMessages.push_back(clone(message));
        return {};
    }

    Result<> sendRequest(const google::protobuf::MessageLite& request, google::protobuf::MessageLite& response) override
    {
        return {};
    }

    mobgtw::MessageBus& messageBus() override { return mMessageBus; }
    const std::optional<mobgtw::SessionInformation>& sessionInfo() const override { return mobgtw::SessionInformation(); }

private:
    mobgtw::MessageBus mMessageBus;

    std::unique_ptr<google::protobuf::MessageLite> clone(const google::protobuf::MessageLite& message)
    {
        std::unique_ptr<google::protobuf::MessageLite> cloned(message.New());
        cloned->CheckTypeAndMergeFrom(message);

        return cloned;
    }
};

}
