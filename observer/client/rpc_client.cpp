//
// Created by zoe50 on 2023/12/12.
//

#include "rpc_client.h"
#include <deque>
#include <atomic>
#include <mutex>
#include <chrono>
#include <grpcpp/grpcpp.h>
#include <vector>
#include "rts_observer.h"
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
static atomic<int> frameCount {0};
static vector<int> rx;

class RtsClient {
public:
    RtsClient(std::shared_ptr<grpc::Channel> channel);
    void Connect();
private:
    std::unique_ptr<message::Rts::Stub> stub_;
};

RtsClient::RtsClient(std::shared_ptr<Channel> channel) : stub_(Rts::NewStub(channel)) {}
void RtsClient::Connect() {
    ClientContext context;
    cout << context.raw_deadline().tv_nsec << endl;
    std::shared_ptr<ClientReaderWriter<ObservationRequest, Message> > stream(
            stub_->ConnectObserver(&context));

    std::thread writer([stream]() {
        while (true) {
            // handle request
            if (command == DISCONNECT) {
                stream->WritesDone();
                command = INVALID_COMMAND;
                return;
            }
            if (command == INVALID_COMMAND) {
                continue;
            }
            ObservationRequest request;
            request.set_command(static_cast<message::Command>(command.load()));
            stream->Write(request);
            sleep_for(microseconds (100));
        }
    });
}



void RpcClient::Connect(const std::string& target) {
    auto rtsClient = RtsClient(grpc::CreateChannel(target, grpc::InsecureChannelCredentials()));
    rtsClient.Connect();
}

void RpcClient::SendCommand(Command cmd){
    while (command != INVALID_COMMAND) {
        sleep_for(microseconds (100));
    }
    command = cmd;
}
