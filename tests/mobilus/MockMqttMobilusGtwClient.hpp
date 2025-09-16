#pragma once

#include "jungi/mobilus_gtw_client/Error.h"
#include "jungi/mobilus_gtw_client/MqttMobilusGtwClient.h"

#include <tl/expected.hpp>

#include <memory>
#include <queue>
#include <vector>

namespace mobmatter::tests::mobilus {

namespace mobgtw = jungi::mobilus_gtw_client;

class MockMqttMobilusGtwClient final : public mobgtw::MqttMobilusGtwClient {
public:
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
        mSentMessages.push_back(clone(message));
        return {};
    }

    Result<> sendRequest(const google::protobuf::MessageLite& request, google::protobuf::MessageLite& response) override
    {
        if (mMockResponses.empty()) {
            return tl::unexpected(mobgtw::Error::Transport("Missing mock response for this request"));
        }

        if (response.GetTypeName() == mMockResponses.front()->GetTypeName()) {
            response.CheckTypeAndMergeFrom(*mMockResponses.front());
        }

        mMockResponses.pop();

        return {};
    }

    void mockResponse(std::unique_ptr<google::protobuf::MessageLite> message)
    {
        mMockResponses.push(std::move(message));
    }

    mobgtw::MessageBus& messageBus() override { return mMessageBus; }
    const std::optional<mobgtw::SessionInformation>& sessionInfo() const override { return mobgtw::SessionInformation(); }

    const std::vector<std::unique_ptr<google::protobuf::MessageLite>>& sentMessages() const { return mSentMessages; }

private:
    mobgtw::MessageBus mMessageBus;
    std::vector<std::unique_ptr<google::protobuf::MessageLite>> mSentMessages;
    std::queue<std::unique_ptr<google::protobuf::MessageLite>> mMockResponses;

    std::unique_ptr<google::protobuf::MessageLite> clone(const google::protobuf::MessageLite& message)
    {
        std::unique_ptr<google::protobuf::MessageLite> cloned(message.New());
        cloned->CheckTypeAndMergeFrom(message);

        return cloned;
    }
};

}
