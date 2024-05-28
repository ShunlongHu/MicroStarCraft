//
// Created by zoe50 on 2023/12/11.
//

#ifndef RTS_RTSSERVICEIMPL_H
#define RTS_RTSSERVICEIMPL_H

#include "message.grpc.pb.h"

class RtsServiceImpl final : public message::Rts::Service {
public:
    static void mainLoop();
    static std::string replayFile;
    static bool isLightOnly;

private:
    grpc::Status ConnectObserver(grpc::ServerContext *context,
                                 grpc::ServerReaderWriter<message::Message, message::ObservationRequest> *stream) final;

    grpc::Status ConnectPlayer(grpc::ServerContext *context,
                               grpc::ServerReaderWriter<message::Message, message::PlayerRequest> *stream) final;

    static void InitReplayStream(std::ifstream &iss);

    static void ServerStep(std::ofstream &ofs, std::ifstream &iss);

    static void StepReplay(std::ifstream &iss);
};


#endif //RTS_RTSSERVICEIMPL_H
