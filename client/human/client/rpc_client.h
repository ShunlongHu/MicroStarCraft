//
// Created by zoe50 on 2023/12/12.
//

#ifndef RTS_RPC_CLIENT_H
#define RTS_RPC_CLIENT_H
#include <string>
#include "message.pb.h"
#include "game_types.h"

enum SpecialCommand {
    INVALID_COMMAND = -1,
    DISCONNECT = 4,
};

namespace RpcClient {
    void Connect(const std::string& target);

    void SendCommand(message::Command cmd);

    std::string GetObservation();

    extern std::atomic<bool> stop;
    extern std::atomic<bool> newState;
}





#endif //RTS_RPC_CLIENT_H