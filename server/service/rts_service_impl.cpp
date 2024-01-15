//
// Created by zoe50 on 2023/12/11.
//

#include "rts_service_impl.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <fstream>
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
static constexpr int TICKING_INTERVAL {200};
static atomic<int> tickingCycle { INT32_MAX };
static atomic<bool> reset {false};
static atomic<bool> tick {false};
static atomic<bool> gameStart {true};
static atomic<bool> serverStart {true};
static mutex stateLock;
static std::atomic<bool> isAConnected {false};
static std::atomic<bool> isBConnected {false};
static ObservationRequest initParam;

static TotalDiscreteAction totalAction;
string RtsServiceImpl::replayFile;

Status RtsServiceImpl::ConnectObserver(ServerContext* context, ServerReaderWriter<Message, ObservationRequest>* stream) {
    cout << "Observer Connected!" << endl;
    atomic<bool> isEnd {false};
    std::thread writer([stream, &isEnd]() {
        Message msg;
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
            initParam = msg;
            tickingCycle = INT32_MAX;
            reset = true;
        }
    }

    writer.join();
    cout << "observer disconnected!" << endl;
    return Status::OK;
}
Status RtsServiceImpl::ConnectPlayer(ServerContext* context, ServerReaderWriter<Message, PlayerRequest>* stream) {
    cout << "Player Connected!" << endl;
    atomic<bool> isEnd {false};
    atomic<int> side {-1};
    std::thread writer([stream, &isEnd, &side]() {
        Message msg;
        GameState lastGameState;
        while (!isEnd) {
            sleep_for(milliseconds(10));
            if (side == -1) {
                continue;
            }
            {
                unique_lock<mutex> lockGuard(stateLock);
                if (lastGameState == GetGameState(0)) {
                    continue;
                }
                lastGameState = GetGameState(0);
                ostringstream oss(ios::binary);
                oss << lastGameState;

                msg.set_data(oss.str());
                stream->Write(msg);
            }
            stream->Write(msg);
        }
    });
    PlayerRequest msg;
    while(stream->Read(&msg)) {
        side = msg.role();
        if (msg.command() == message::DISCONNECT) {
            cout << "player " << static_cast<char>('A' + static_cast<char>(msg.role())) << " disconnect!" << endl;
            isEnd = true;
            continue;
        }
        if (msg.command() == message::RESET) {
            cout << "player " << static_cast<char>('A' + static_cast<char>(msg.role())) << " echo!" << endl;
            continue;
        }
//        cout << "player " << static_cast<char>('A' + static_cast<char>(msg.role())) << " act!" << endl;
        unique_lock<mutex> lockGuard(stateLock);
        auto& act = totalAction.action[msg.role()];
        act.clear();
        act.reserve(msg.actions_size());
        for (const auto& a: msg.actions()) {
            act.emplace(a.id(), DiscreteAction{static_cast<ActionType>(a.action()), static_cast<GameObjType>(a.producetype()), {a.targety(), a.targetx()}});
        }
    }
    writer.join();
    cout << "player " << static_cast<char>('A' + static_cast<char>(side)) << " disconnect!" << endl;
    return Status::OK;
}

void RtsServiceImpl::mainLoop() {
    ofstream ofs;
    istringstream iss;
    if (replayFile.empty()) {
        tm newTime {};
        time_t nowTime = std::time(nullptr);
        localtime_s(&newTime,&nowTime);
        ostringstream oss;
        oss << std::put_time(&newTime, "%Y%m%d_%H%M%S");
        ofs.open(oss.str()+".replay", ios::binary);
    } else {
        InitReplayStream(iss);
    }
    static auto time = chrono::high_resolution_clock::now();
    while (serverStart) {
        sleep_for(microseconds (100));
        unique_lock<mutex> lockGuard(stateLock);
        if (reset) {
            if (!replayFile.empty()) {
                InitReplayStream(iss);
                StepReplay(iss);
            } else {
                cout << "seed: " << initParam.seed()
                     << " isRotSym: " << initParam.isrotsym()
                     << " isAxSym: " << !initParam.isrotsym()
                     << " terrainProb: " << initParam.terrainprob()
                     << " expansionCnt: " << initParam.expansioncnt()
                     << " clusterPerExpansion: " << initParam.clusterperexpansion()
                     << " mineralPerCluster: " << initParam.mineralpercluster() << endl;

                (void) Reset(initParam.seed(),
                             initParam.isrotsym(),
                             !initParam.isrotsym(),
                             initParam.terrainprob(),
                             initParam.expansioncnt(),
                             initParam.clusterperexpansion(),
                             initParam.mineralpercluster());
                GetGameState(0).time = 0;
                ofs << GetGameState(0);
            }
            reset = false;
        }
        if (tick) {
            ServerStep(ofs, iss);
            tick = false;
        }
        if (gameStart) {
            auto now = chrono::high_resolution_clock::now();
            if (duration_cast<milliseconds>(now - time).count() >= tickingCycle) {
                time = now;
                ServerStep(ofs, iss);
            }
        }
        if (GetGameState(0).buildingCnt[0] == 0 || GetGameState(0).buildingCnt[1] == 0 || !iss) {
            tick = false;
            tickingCycle = INT32_MAX;
            ofs.flush();
        }
    }
}

void RtsServiceImpl::StepReplay(istringstream &iss) {
    if (iss.good() && iss.tellg() < iss.str().size()) {
        try {
            iss >> GetGameState(0);
        } catch(exception& e){
            cerr << "Failed to read from replay" << endl;
        }
    }
}

void RtsServiceImpl::ServerStep(ofstream &ofs, istringstream &iss) {
    if (!replayFile.empty()) {
        StepReplay(iss);
    } else {
        Step(totalAction);
        ofs << GetGameState(0);
    }
}

void RtsServiceImpl::InitReplayStream(istringstream &iss) {
    ifstream ifs(replayFile, ios::binary);
    string str(istreambuf_iterator<char>(ifs), {});
    iss = istringstream(str);
    cout << "Reading replay: " << replayFile << endl;
    cout << "Replay size: " << str.size() << endl;
}
