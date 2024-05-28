//
// Created by zoe50 on 2023/12/9.
//
/*
 *
 * Copyright 2015 gRPC authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <iostream>
#include <memory>
#include <string>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/strings/str_format.h"

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include "rts_service_impl.h"
#include "client_interface.h"

ABSL_FLAG(uint16_t, port, 50051, "Server port for the service");
ABSL_FLAG(std::string, replay, "", "Replay file path");
ABSL_FLAG(bool, light_only, false, "Cannot build heavy and ranged");

using grpc::Server;
using grpc::ServerBuilder;
using grpc::Status;

// Logic and data behind the server's behavior.

void RunServer(uint16_t port) {
    std::string server_address = absl::StrFormat("0.0.0.0:%d", port);
    RtsServiceImpl service;

    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *synchronous* service.
    builder.RegisterService(&service);
    // Finally assemble the server.
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    std::thread mainloop (RtsServiceImpl::mainLoop);
    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
    mainloop.join();
}

int main(int argc, char** argv) {
    absl::ParseCommandLine(argc, argv);
    RtsServiceImpl::replayFile = absl::GetFlag(FLAGS_replay);
    RtsServiceImpl::isLightOnly = absl::GetFlag(FLAGS_light_only);
    Init({W, H, 1});
    RunServer(absl::GetFlag(FLAGS_port));
    return 0;
}