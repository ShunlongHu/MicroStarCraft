//
// Created by zoe50 on 2023/12/11.
//

#include "rts_service_impl.h"
#include <chrono>

using grpc::Status;
using grpc::ServerContext;
using grpc::ServerWriter;
using grpc::ServerReaderWriter;
using message::ObservationRequest;
using message::PlayerRequest;
using message::Message;
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds

Status RtsServiceImpl::ConnectObserver(ServerContext* context, const ObservationRequest* request, ServerWriter<Message>* writer) {
    Message msg;
    msg.mutable_msg()->append("hello");
    while(true) {
        writer->Write(msg);
        sleep_for(milliseconds (500));
    }

    return Status::OK;
}
Status RtsServiceImpl::ConnectPlayer(ServerContext* context, ServerReaderWriter<Message, PlayerRequest>* stream) {
    Message msg;
    msg.mutable_msg()->append("hello");
    while(true) {
        stream->Write(msg);
        sleep_for(milliseconds (500));
    }
    return Status::OK;
}