//
// Created by zoe50 on 2023/12/9.
//

#include "client_interface.h"
#include <vector>
#include <iostream>
#include <chrono>
#include <random>
#include <algorithm>
#include "thread_pool.h"
#include "game_engine.h"
#include "state_to_observation.h"

using namespace std;
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds

constexpr static int INIT_RESOURCE = 10;

static vector<signed char> observationVec[2]; // NUM_WORKER * FLATTENED_FEATURE
static vector<int> rewardVec[2]; // NUM_WORKER * FEATURE
static vector<GameState> gameStateVec;
static unique_ptr<ThreadPool> pool;
static atomic<int> taskCounter;
static TotalObservation totalObservation;

void
ResetThread(int idx, int seed, bool isRotSym, bool isAxSym, double terrainProb, int expansionCnt,
            int clusterPerExpansion,
            int mineralPerCluster) {
    auto &game = gameStateVec[idx];
    game.time = 0;
    game.resource[0] = INIT_RESOURCE;
    game.resource[1] = INIT_RESOURCE;
    game.buildingCnt[0] = 1;
    game.buildingCnt[1] = 1;
    game.objMap = {};

    const int MINERAL_DISTANCE = 2;
    vector<Coord> expansionVec;
    default_random_engine engine(seed);
    uniform_int_distribution<int> dist(0, game.w * game.h / 2 - 1);
    uniform_int_distribution<int> direction(0, 3);
    for (int i = 0; i < expansionCnt; ++i) {
        int loc;
        int x;
        int y;
        bool isValidExpansion;
        int rollCnt = 0;
        do {
            loc = dist(engine);
            x = loc % game.w;
            y = loc / game.w;
            isValidExpansion = true;
            if (x < MINERAL_DISTANCE || x >= game.w - MINERAL_DISTANCE || y < MINERAL_DISTANCE || y >= game.h / 2 - MINERAL_DISTANCE) {
                isValidExpansion = false;
            }
            for (const auto& expansion: expansionVec) {
                if (abs(expansion.x - x) <= 2 * MINERAL_DISTANCE && abs(expansion.y - y) <= 2 * MINERAL_DISTANCE) {
                    isValidExpansion = false;
                    break;
                }
            }
        } while (!isValidExpansion && ++rollCnt < 100); // roll 100 time maximum to avoid infinite loop
        if (rollCnt >= 100) {
            break;
        }
        if (expansionVec.empty()) {
            GameObj base{BASE};
            base.resource = INIT_RESOURCE;
            game.objMap.emplace(Coord{y, x}, base);
        }
        expansionVec.emplace_back(y, x);
        for (int tileY = -MINERAL_DISTANCE; tileY <= MINERAL_DISTANCE; ++tileY) {
            for (int tileX = -MINERAL_DISTANCE; tileX <= MINERAL_DISTANCE; ++tileX) {
                auto tileLoc = (tileY + y) * game.w + tileX + x;
            }
        }
        auto dir = direction(engine);
        const static vector<vector<Coord>> DIRECTION_OFFSET_MAP {
                {{-2,-2},{-2,-1},{-2,0},{-2,1},{-2,2}},
                {{2,-2},{2,-1},{2,0},{2,1},{2,2}},
                {{-2,-2},{-1,-2},{0,-2},{1,-2},{2,-2}},
                {{-2,2},{-1,2},{0,2},{1,2},{2,2}}};
        for (int exp = 0; exp < clusterPerExpansion; ++exp) {
            const auto &coord = DIRECTION_OFFSET_MAP[dir][exp];
            game.objMap.emplace(Coord{y + coord.y, x + coord.x}, GameObj{MINERAL});
        }
    }
    int remainingTileCnt = game.h * game.w / 2 - (MINERAL_DISTANCE * 2 + 1) * (MINERAL_DISTANCE * 2 + 1) * expansionCnt;
    int terrainCnt = remainingTileCnt * terrainProb;
    vector<bool> terrainFlagVec(terrainCnt, true);
    terrainFlagVec.resize(remainingTileCnt, false);
    shuffle(terrainFlagVec.begin(), terrainFlagVec.end(), engine);
    int validIdx = 0;
    for (int i = 0; i < game.w * game.h / 2; ++i){
        auto x = i % game.w;
        auto y = i / game.h;
        bool isValidTerrain = true;
        for (const auto& expansion: expansionVec) {
            if (abs(expansion.x - x) <= MINERAL_DISTANCE && abs(expansion.y - y) <= MINERAL_DISTANCE) {
                isValidTerrain = false;
                break;
            }
        }
        if (!isValidTerrain) {
            continue;
        }
        if (!terrainFlagVec[validIdx++]) {
            continue;
        }
        game.objMap.emplace(Coord{y, x}, GameObj{TERRAIN});
    }
    for (auto& [_, obj]: game.objMap) {
        if (obj.type == MINERAL) {
            obj.resource = mineralPerCluster;
        }
        if (obj.type == BASE) {
            obj.owner = -1;
            obj.hitPoint = OBJ_HP_MAP.at(obj.type);
        } else {
            obj.owner = 0;
        }
        obj.currentAction = NOOP;
        obj.actionTarget = {0,0};
        obj.actionProgress = 0;
        obj.actionTotalProgress = 0;
        obj.attackCD = 0;
        obj.attackRange = 0;
        obj.attackInterval = 0;
        obj.attackPoint = 0;
        obj.moveInterval = 0;
        obj.actionMask = OBJ_ACTION_MASK_MAP.at(obj.type);
    }
    auto cpy = game.objMap;
    for (const auto& [loc, obj]: cpy) {
        auto newLoc = loc;
        auto newObj = obj;
        newLoc.y = game.h - newLoc.y - 1;
        if (isRotSym) {
            newLoc.x = game.w - newLoc.x - 1;
        }
        if (newObj.owner == -1) {
            newObj.owner = 1;
        }
        game.objMap.emplace(newLoc, newObj);
    }
    taskCounter++;
}

extern "C" __declspec(dllexport) void Init(InitParam initParam) {
    cout << "Init " << initParam.w << "x" << initParam.h << " with " << initParam.numWorkers << " workers." << endl;
    gameStateVec.resize(initParam.numWorkers, {{}, {0, 0}, {1, 1}, initParam.w, initParam.h, 0});

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
        sleep_for(microseconds(100));
    }

    taskCounter = 0;
    for (int i = 0; i < gameStateVec.size(); ++i) {
        pool->enqueue(StateToObservation, &gameStateVec[i], observationVec, rewardVec, i, &taskCounter);
    }
    while (taskCounter != gameStateVec.size()) {
        sleep_for(microseconds(100));
    }

    return totalObservation;
}

GameState &GetGameState(int gameIdx) {
    return gameStateVec[gameIdx];
}

extern "C" __declspec(dllexport) TotalObservation
Reset(int seed, bool isRotSym, bool isAxSym, double terrainProb, int expansionCnt, int clusterPerExpansion,
      int mineralPerCluster) {
    taskCounter = 0;
    for (int i = 0; i < gameStateVec.size(); ++i) {
        pool->enqueue(ResetThread, i, seed, isRotSym, isAxSym, terrainProb / 100, expansionCnt, clusterPerExpansion,
                      mineralPerCluster);
    }
    while (taskCounter != gameStateVec.size()) {
        sleep_for(microseconds(100));
    }
    return totalObservation;
}