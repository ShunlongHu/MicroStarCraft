//
// Created by zoe50 on 2023/12/9.
//

#include "client_interface.h"
#include <vector>
#include <iostream>
#include <chrono>
#include "thread_pool.h"
#include "game_engine.h"
#include "state_to_observation.h"

using namespace std;
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds

static vector<signed char> observationVec[2]; // NUM_WORKER * FLATTENED_FEATURE
static vector<int> rewardVec[2]; // NUM_WORKER * FEATURE
static vector<GameState> gameStateVec;
static unique_ptr<ThreadPool> pool;
static atomic<int> taskCounter;
static TotalObservation totalObservation;

extern "C" __declspec(dllexport) void Init(InitParam initParam) {
    cout << "Init " << initParam.w << "x" << initParam.h << " with " << initParam.numWorkers << " workers." << endl;
    gameStateVec.resize(initParam.numWorkers,{{}, initParam.w, initParam.h});

    auto observationSize = initParam.numWorkers * initParam.h * initParam.w * OBSERVATION_PLANE_NUM;
    observationVec[0].resize(observationSize);
    observationVec[1].resize(observationSize);

    auto featureSize = initParam.numWorkers * GAME_STAT_NUM;
    rewardVec[0].resize(featureSize);
    rewardVec[1].resize(featureSize);

    for (int i = 0; i < initParam.numWorkers; ++i) {
        totalObservation.ob1.size = static_cast<int>(gameStateVec.size());
        totalObservation.ob1.data = observationVec[0].data();
        totalObservation.ob1.reward = rewardVec[0].data();

        totalObservation.ob2.size = static_cast<int>(gameStateVec.size());
        totalObservation.ob2.data = observationVec[0].data();
        totalObservation.ob2.reward = rewardVec[0].data();
    }
    pool = make_unique<ThreadPool>(initParam.numWorkers);
}

extern "C" __declspec(dllexport) TotalObservation Step(TotalAction totalAction) {
    taskCounter = 0;
    for (int i = 0; i < gameStateVec.size(); ++i) {
        pool->enqueue(GameStep, &gameStateVec[i], &taskCounter);
    }
    while (taskCounter != gameStateVec.size()) {
        sleep_for(microseconds (100));
    }

    taskCounter = 0;
    for (int i = 0; i < gameStateVec.size(); ++i) {
        pool->enqueue(StateToObservation, &gameStateVec[i], observationVec, rewardVec, i, &taskCounter);
    }
    while (taskCounter != gameStateVec.size()) {
        sleep_for(microseconds (100));
    }

    return totalObservation;
}

GameState &GetGameState(int gameIdx) {
    return gameStateVec[gameIdx];
}