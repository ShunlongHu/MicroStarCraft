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
static GameState state;
static unordered_map<int, DiscreteAction> discreteAction;
static mutex stateLock;
static mutex actionLock;
atomic<bool> RpcClient::stop { true };
static vector<int> rx;
atomic<bool> RpcClient::newState { false };

class RtsObserverClient {
public:
    explicit RtsObserverClient(const std::shared_ptr<grpc::Channel>& channel);
    void Connect();
private:
    std::unique_ptr<message::Rts::Stub> stub_;
    std::shared_ptr<Channel> channel_;
};

class RtsPlayerClient {
public:
    explicit RtsPlayerClient(const std::shared_ptr<grpc::Channel>& channel, ::Role role);
    void Connect();
private:
    std::unique_ptr<message::Rts::Stub> stub_;
    std::shared_ptr<Channel> channel_;
    message::Role role;
};

RtsObserverClient::RtsObserverClient(const std::shared_ptr<Channel>& channel) : stub_(Rts::NewStub(channel)), channel_(channel){}
void RtsObserverClient::Connect() {
    ClientContext context;
    std::shared_ptr<ClientReaderWriter<ObservationRequest, Message> > stream(
            stub_->ConnectObserver(&context));
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
            ObservationRequest request;
            request.set_command(static_cast<message::Command>(command.load()));
            if (command == RESET) {
                request.set_clusterperexpansion(RtsObserver::clusterCnt);
                request.set_mineralpercluster(RtsObserver::resourceCnt);
                request.set_expansioncnt(RtsObserver::expansionCnt);
                request.set_terrainprob(RtsObserver::terrainProb);
                request.set_seed(RtsObserver::seed);
                request.set_isaxsym(RtsObserver::isAxSym);
                request.set_isrotsym(!RtsObserver::isAxSym);
            }
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
        istringstream iss(result.data(), ios::binary);
        unique_lock<mutex> lockGuard(stateLock);
        iss >> state;
        newState = true;
    }
    writer.join();
}

RtsPlayerClient::RtsPlayerClient(const std::shared_ptr<Channel>& channel, ::Role role) : stub_(Rts::NewStub(channel)), channel_(channel){
    if (role == ::Role::PLAYER_A) {
        this->role = message::PLAYER_A;
    } else {
        this->role = message::PLAYER_B;
    }
}
void RtsPlayerClient::Connect() {
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
            request.set_role(role);
            {
                unique_lock<mutex> lockGuard(actionLock);
                request.set_command(static_cast<message::Command>(command.load()));
                request.mutable_actions()->Reserve(discreteAction.size());
                for (const auto& [id, act]: discreteAction) {
                    auto ptr = request.add_actions();
                    ptr->set_id(id);
                    ptr->set_action(act.action);
                    ptr->set_producetype(act.produceType);
                    ptr->set_targetx(act.target.x);
                    ptr->set_targety(act.target.y);
                }
            }
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
        istringstream iss(result.data(), ios::binary);
        unique_lock<mutex> lockGuard(stateLock);
        iss >> state;
        newState = true;
    }
    writer.join();
}

void RpcClient::Connect(const std::string& target, ::Role role) {
    if (role == ::Role::OBSERVER) {
        auto rtsClient = RtsObserverClient(grpc::CreateChannel(target, grpc::InsecureChannelCredentials()));
        rtsClient.Connect();
    } else {
        auto rtsClient = RtsPlayerClient(grpc::CreateChannel(target, grpc::InsecureChannelCredentials()), role);
        rtsClient.Connect();
    }
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

GameState RpcClient::GetObservation() {
    unique_lock<mutex> lockGuard(stateLock);
    return state;
};

void RpcClient::SetAction(const std::unordered_map<int, DiscreteAction>& action) {
    unique_lock<mutex> lockGuard(actionLock);
    discreteAction = action;
}