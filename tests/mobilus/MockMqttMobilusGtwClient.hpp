#pragma once

#include <jungi/mobgtw/Error.h>
#include <jungi/mobgtw/MqttMobilusGtwClient.h>
#include <jungi/mobgtw/proto/CurrentStateRequest.pb.h>
#include <jungi/mobgtw/proto/CurrentStateResponse.pb.h>
#include <jungi/mobgtw/proto/DeviceSettingsRequest.pb.h>
#include <jungi/mobgtw/proto/DeviceSettingsResponse.pb.h>
#include <jungi/mobgtw/proto/DevicesListRequest.pb.h>
#include <jungi/mobgtw/proto/DevicesListResponse.pb.h>
#include <jungi/mobgtw/proto/NetworkSettingsRequest.pb.h>
#include <jungi/mobgtw/proto/NetworkSettingsResponse.pb.h>
#include <jungi/mobgtw/proto/UpdateDeviceRequest.pb.h>
#include <jungi/mobgtw/proto/UpdateDeviceResponse.pb.h>
#include <tl/expected.hpp>

#include <memory>
#include <queue>
#include <vector>

using namespace jungi::mobgtw;

namespace mobmatter::tests::mobilus {

class MockMqttMobilusGtwClient final : public MqttMobilusGtwClient {
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

    Result<> sendRequest(const proto::CurrentStateRequest& request, proto::CurrentStateResponse& response) override { return sendMockResponseFor(request, response); }
    Result<> sendRequest(const proto::DeviceSettingsRequest& request, proto::DeviceSettingsResponse& response) override { return sendMockResponseFor(request, response); }
    Result<> sendRequest(const proto::DevicesListRequest& request, proto::DevicesListResponse& response) override { return sendMockResponseFor(request, response); }
    Result<> sendRequest(const proto::NetworkSettingsRequest& request, proto::NetworkSettingsResponse& response) override { return sendMockResponseFor(request, response); }
    Result<> sendRequest(const proto::UpdateDeviceRequest& request, proto::UpdateDeviceResponse& response) override { return sendMockResponseFor(request, response); }

    void mockResponse(std::unique_ptr<google::protobuf::MessageLite> message)
    {
        mMockResponses.push(std::move(message));
    }

    MessageBus& messageBus() override { return mMessageBus; }
    const std::optional<SessionInformation>& sessionInfo() const override { return SessionInformation {}; }

    const std::vector<std::unique_ptr<google::protobuf::MessageLite>>& sentMessages() const { return mSentMessages; }
    const std::vector<std::unique_ptr<google::protobuf::MessageLite>>& sentRequests() const { return mSentRequests; }

private:
    MessageBus mMessageBus;
    std::vector<std::unique_ptr<google::protobuf::MessageLite>> mSentMessages;
    std::vector<std::unique_ptr<google::protobuf::MessageLite>> mSentRequests;
    std::queue<std::unique_ptr<google::protobuf::MessageLite>> mMockResponses;

    Result<> sendMockResponseFor(const google::protobuf::MessageLite& request, google::protobuf::MessageLite& response)
    {
        std::unique_ptr<google::protobuf::MessageLite> sentRequest(request.New());

        sentRequest->CheckTypeAndMergeFrom(request);
        mSentRequests.push_back(std::move(sentRequest));

        if (mMockResponses.empty()) {
            return tl::unexpected(Error::Transport("Missing mock response for this request"));
        }

        if (response.GetTypeName() == mMockResponses.front()->GetTypeName()) {
            response.CheckTypeAndMergeFrom(*mMockResponses.front());
        }

        mMockResponses.pop();

        return {};
    }

    std::unique_ptr<google::protobuf::MessageLite> clone(const google::protobuf::MessageLite& message)
    {
        std::unique_ptr<google::protobuf::MessageLite> cloned(message.New());
        cloned->CheckTypeAndMergeFrom(message);

        return cloned;
    }
};

}
