//
// Created by zoe50 on 2024/1/18.
//
#include <iostream>
#include <memory>
#include <string>
#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/strings/str_format.h"
#include "strategy.h"

using namespace std;
using namespace message;

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using grpc::ClientReaderWriter;
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds
using namespace message;

ABSL_FLAG(std::string, address, "127.0.0.1", "Server ip");
ABSL_FLAG(uint16_t, port, 50051, "Server port for the service");
ABSL_FLAG(std::string, side, "PLAYER_A", "Player Side");

static GameState state;
static unordered_map<int, DiscreteAction> action;
static Role role;
unique_ptr<message::Rts::Stub> stub_;
shared_ptr<Channel> channel_;

int main(int argc, char** argv) {
    absl::ParseCommandLine(argc, argv);
    string server_address = absl::GetFlag(FLAGS_address) + absl::StrFormat(":%d", absl::GetFlag(FLAGS_port));
    channel_ = grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials());
    stub_ = Rts::NewStub(channel_);
    ClientContext context;
    std::shared_ptr<ClientReaderWriter<PlayerRequest, Message> > stream(
            stub_->ConnectPlayer(&context));
    if (channel_->GetState(false) != GRPC_CHANNEL_READY) {
        cerr << "Failed to connect to: " << server_address << endl;
        return 1;
    }
    if (absl::GetFlag(FLAGS_side) == "PLAYER_A") {
        role = PLAYER_A;
    } else if (absl::GetFlag(FLAGS_side) == "PLAYER_B") {
        role = PLAYER_B;
    } else {
        cerr << "Side must be: 'PLAYER_A' or 'PLAYER_B'" << endl;
        return 1;
    }

    Message result;
    PlayerRequest startRequest;
    startRequest.set_role(role);
    stream->Write(startRequest);
    while (stream->Read(&result)) {
        istringstream iss(result.data(), ios::binary);
        iss >> state;
        Strategy::Act(state, action);
        PlayerRequest request;
        request.set_role(role);
        request.set_command(STEP);
        request.mutable_actions()->Reserve(action.size());
        for (const auto& [id, act]: action) {
            auto ptr = request.add_actions();
            ptr->set_id(id);
            ptr->set_action(act.action);
            ptr->set_producetype(act.produceType);
            ptr->set_targetx(act.target.x);
            ptr->set_targety(act.target.y);
        }
        stream->Write(request);
        action = {};
    }
    return 0;
}

