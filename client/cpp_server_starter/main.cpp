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
ABSL_FLAG(int32_t, seed, 0, "seed");
ABSL_FLAG(bool, isAxSym, false, "is axial symmetric");
ABSL_FLAG(bool, isRotSym, true, "is rotational symmetric");
ABSL_FLAG(double, terrainProb, 0, "terrain probability");
ABSL_FLAG(int32_t, expansionCnt, 1, "expansions num");
ABSL_FLAG(int32_t, clusterPerExpansion, 1, "clusters per expansion");
ABSL_FLAG(int32_t, mineralPerCluster, 15, "minerals per cluster");

static Role role;
unique_ptr<message::Rts::Stub> stub_;
shared_ptr<Channel> channel_;

int main(int argc, char** argv) {
    absl::ParseCommandLine(argc, argv);
    auto seedFlag = absl::GetFlag(FLAGS_seed);
    auto isAxSymFlag = absl::GetFlag(FLAGS_isAxSym);
    auto isRotSymFlag = absl::GetFlag(FLAGS_isRotSym);
    auto terrainProbFlag = absl::GetFlag(FLAGS_terrainProb);
    auto expansionCntFlag = absl::GetFlag(FLAGS_expansionCnt);
    auto clusterPerExpansionFlag = absl::GetFlag(FLAGS_clusterPerExpansion);
    auto mineralPerCluster = absl::GetFlag(FLAGS_mineralPerCluster);

    if (!(isAxSymFlag ^ isRotSymFlag)) {
        cerr << "The map must be either Axial Symmetric or Rotational Symmetric" << endl;
        return 1;
    }
    if (terrainProbFlag < 0 || terrainProbFlag > 1) {
        cerr << "Terrain Probability must be between 0-1" << endl;
        return 1;
    }
    if (expansionCntFlag < 1 || expansionCntFlag > 5) {
        cerr << "Expansion count must be between 1-5" << endl;
        return 1;
    }
    if (clusterPerExpansionFlag < 1 || clusterPerExpansionFlag > 5) {
        cerr << "Cluster per expansion must be between 1-5" << endl;
        return 1;
    }
    if (mineralPerCluster < 15 || mineralPerCluster > 120) {
        cerr << "Minerals per cluster must be between 15-120" << endl;
        return 1;
    }


    string server_address = absl::GetFlag(FLAGS_address) + absl::StrFormat(":%d", absl::GetFlag(FLAGS_port));
    channel_ = grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials());
    stub_ = Rts::NewStub(channel_);
    ClientContext context;
    std::shared_ptr<ClientReaderWriter<ObservationRequest, Message> > stream(
            stub_->ConnectObserver(&context));
    if (channel_->GetState(false) != GRPC_CHANNEL_READY) {
        cerr << "Failed to connect to: " << server_address << endl;
        return 1;
    }

    Message result;
    ObservationRequest request;
    sleep_for(milliseconds (100));
    request.set_clusterperexpansion(clusterPerExpansionFlag);
    request.set_mineralpercluster(mineralPerCluster);
    request.set_expansioncnt(expansionCntFlag);
    request.set_terrainprob(terrainProbFlag * 100);
    request.set_seed(seedFlag);
    request.set_isaxsym(isAxSymFlag);
    request.set_isrotsym(!isAxSymFlag);
    request.set_command(RESET);
    stream->Write(request);
    request.set_command(START);
    stream->Write(request);
    sleep_for(milliseconds (100));
    stream->WritesDone();
    return 0;
}

