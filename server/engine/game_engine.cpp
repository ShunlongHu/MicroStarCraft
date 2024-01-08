//
// Created by zoe50 on 2023/12/11.
//

#include "game_engine.h"
#include <random>
#include <algorithm>
constexpr static int INIT_RESOURCE = 10;

using namespace std;
void GameReset(GameState *ptrGameState, int seed, bool isRotSym, bool isAxSym, double terrainProb, int expansionCnt,
               int clusterPerExpansion, int mineralPerCluster, std::atomic<int> *ptrCounter) {
    auto& taskCounter = *ptrCounter;
    auto &game = *ptrGameState;
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
            GameObj base{BASE, {y,x}};
            base.resource = INIT_RESOURCE;
            game.objMap.emplace(game.objCnt++, base);
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
            game.objMap.emplace(game.objCnt++, GameObj{MINERAL, Coord{y + coord.y, x + coord.x}});
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
        game.objMap.emplace(game.objCnt++, GameObj{TERRAIN, {y, x}});
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
    for (const auto& [_, obj]: cpy) {
        auto newLoc = obj.coord;
        auto newObj = obj;
        newLoc.y = game.h - newLoc.y - 1;
        if (isRotSym) {
            newLoc.x = game.w - newLoc.x - 1;
        }
        if (newObj.owner == -1) {
            newObj.owner = 1;
        }
        newObj.coord = newLoc;
        game.objMap.emplace(game.objCnt++, newObj);
    }
    taskCounter++;
}

void GameStep(GameState* ptrGameState, atomic<int>* ptrCounter) {
    auto& game = *ptrGameState;
    auto& counter = *ptrCounter;

    counter++;
}

void GameStepSingle(GameState& game, TotalDiscreteAction action) {
    game.time++;
}