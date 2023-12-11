//
// Created by zoe50 on 2023/12/11.
//

#ifndef RTS_RTSSERVICEIMPL_H
#define RTS_RTSSERVICEIMPL_H

#include "message.grpc.pb.h"

class RtsServiceImpl final : public message::Rts::Service {
    grpc::Status ConnectObserver(grpc::ServerContext *context, const message::ObservationRequest *request,
                                 grpc::ServerWriter<message::Message> *writer);

    grpc::Status ConnectPlayer(grpc::ServerContext *context,
                               grpc::ServerReaderWriter<message::Message, message::PlayerRequest> *stream);
};


#endif //RTS_RTSSERVICEIMPL_H
