//
// Created by zoe50 on 2023/12/12.
//

#ifndef RTS_RPC_CLIENT_H
#define RTS_RPC_CLIENT_H
#include <string>
#include "message.pb.h"

enum SpecialCommand {
    INVALID_COMMAND = -1,
    DISCONNECT = 4,
};

namespace RpcClient {
    void Connect(const std::string& target);

    void SendCommand(message::Command cmd);

    int GetObservation();

}





#endif //RTS_RPC_CLIENT_H
