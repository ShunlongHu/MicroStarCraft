//
// Created by zoe50 on 2023/12/11.
//

#include "rts_service_impl.h"
#include <chrono>
#include <sstream>
#include <mutex>
#include "client_interface.h"

using grpc::Status;
using grpc::ServerContext;
using grpc::ServerWriter;
using grpc::ServerReaderWriter;
using message::ObservationRequest;
using message::PlayerRequest;
using message::Message;
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds
using namespace std;
static constexpr int TICKING_INTERVAL {100};
static atomic<int> tickingCycle { INT32_MAX };
static atomic<bool> reset {false};
static atomic<bool> tick {false};
static atomic<bool> gameStart {true};
static atomic<bool> serverStart {true};
static mutex stateLock;

Status RtsServiceImpl::ConnectObserver(ServerContext* context, ServerReaderWriter<Message, ObservationRequest>* stream) {
    cout << "Connected!" << endl;
    atomic<bool> isEnd {false};
    std::thread writer([stream, &isEnd]() {
        Message msg;
        msg.mutable_msg()->append("hello");
        GameState lastGameState;
        while (!isEnd) {
            sleep_for(milliseconds(10));
            {
                unique_lock<mutex> lockGuard(stateLock);
                if (lastGameState == GetGameState(0)) {
                    continue;
                }
                lastGameState = GetGameState(0);
            }
            ostringstream oss(ios::binary);
            oss << lastGameState;

            msg.set_data(oss.str());
            stream->Write(msg);
        }
    });
    ObservationRequest msg;
    while(stream->Read(&msg)) {
        if (msg.command() == message::DISCONNECT) {
            cout << "observer disconnect!" << endl;
            isEnd = true;
        }
        if (msg.command() == message::START) {
            cout << "observer cmd start!" << endl;
            tickingCycle = TICKING_INTERVAL;
        }
        if (msg.command() == message::STOP) {
            cout << "observer cmd stop!" << endl;
            tickingCycle = INT32_MAX;
        }
        if (msg.command() == message::STEP) {
            cout << "observer cmd step!" << endl;
            tick = true;
        }
        if (msg.command() == message::RESET) {
            cout << "observer cmd reset!" << endl;
            reset = true;
        }
    }

    writer.join();
    cout << "observer disconnected!" << endl;
    return Status::OK;
}
Status RtsServiceImpl::ConnectPlayer(ServerContext* context, ServerReaderWriter<Message, PlayerRequest>* stream) {
    Message msg;
    msg.set_msg("hello!");
    while(true) {
        stream->Write(msg);
        sleep_for(milliseconds (500));
    }
    return Status::OK;
}

void RtsServiceImpl::mainLoop() {
    static auto time = chrono::high_resolution_clock::now();
    while (serverStart) {
        sleep_for(microseconds (100));
        unique_lock<mutex> lockGuard(stateLock);
        if (reset) {
            GetGameState(0).time = 0;
            reset = false;
        }
        if (tick) {
            GetGameState(0).time++;
            tick = false;
        }
        if (gameStart) {
            auto now = chrono::high_resolution_clock::now();
            if (duration_cast<milliseconds>(now - time).count() >= tickingCycle) {
                time = now;
                GetGameState(0).time++;
            }
        }
    }
}
