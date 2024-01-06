//
// Created by zoe50 on 2023/12/12.
//

#include "rpc_client.h"
#include <deque>
#include <atomic>
#include <mutex>
#include <chrono>
#include <sstream>
#include <grpcpp/grpcpp.h>
#include <vector>
#include "rts_human.h"
#include "message.grpc.pb.h"

using namespace std;
using namespace message;

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using grpc::ClientReaderWriter;
using namespace RpcClient;
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds
using namespace message;

static atomic<int> command {INVALID_COMMAND};
static int role = 0;
static string action;
static string ob;
static mutex stateLock;
atomic<bool> RpcClient::stop { true };
static vector<int> rx;
atomic<bool> RpcClient::newState { false };

class RtsClient {
public:
    explicit RtsClient(const std::shared_ptr<grpc::Channel>& channel);
    void Connect();
private:
    std::unique_ptr<message::Rts::Stub> stub_;
    std::shared_ptr<Channel> channel_;
};

RtsClient::RtsClient(const std::shared_ptr<Channel>& channel) : stub_(Rts::NewStub(channel)), channel_(channel){}
void RtsClient::Connect() {
    ClientContext context;
    std::shared_ptr<ClientReaderWriter<PlayerRequest, Message> > stream(
            stub_->ConnectPlayer(&context));
    if (channel_->GetState(false) != GRPC_CHANNEL_READY) {
        return;
    }
    stop = false;
    std::thread writer([this, stream]() {
        while (true) {
            if (channel_->GetState(false) != GRPC_CHANNEL_READY) {
                stop = true;
                return;
            }
            // handle request
            if (command == INVALID_COMMAND) {
                sleep_for(microseconds (100));
                continue;
            }
            PlayerRequest request;
            request.set_command(static_cast<message::Command>(command.load()));
            request.set_role(role == 0 ? Role::PLAYER_A : Role::PLAYER_B);
            request.set_data(action);
            stream->Write(request);
            if (command == SpecialCommand::DISCONNECT) {
                stream->WritesDone();
                command = INVALID_COMMAND;
                return;
            }
            command = INVALID_COMMAND;
            sleep_for(microseconds (100));
        }
    });

    Message result;

    while (!RpcClient::stop && stream->Read(&result)) {
        ob.resize(result.data().size());
        for (int i = 0; i < result.data().size(); ++i) {
            ob[i] = result.data()[i];
        }
        newState = true;
    }
    writer.join();
}



void RpcClient::Connect(const std::string& target) {
    auto rtsClient = RtsClient(grpc::CreateChannel(target, grpc::InsecureChannelCredentials()));
    rtsClient.Connect();

}

void RpcClient::SendCommand(Command cmd){
    if (stop) {
        return;
    }
    while (command != INVALID_COMMAND) {
        sleep_for(microseconds (100));
    }
    command = cmd;
    while (command != INVALID_COMMAND) {
        sleep_for(microseconds (100));
    }
}

string RpcClient::GetObservation() {
    unique_lock<mutex> lockGuard(stateLock);
    return ob;
};