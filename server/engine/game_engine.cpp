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
        obj.actionTarget = obj.coord;
        obj.actionProgress = 0;
        obj.actionTotalProgress = 0;
        obj.attackCD = 0;
        obj.attackRange = 0;
        obj.attackInterval = 0;
        obj.attackPoint = 0;
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
        newObj.actionTarget = newObj.coord;
        game.objMap.emplace(game.objCnt++, newObj);
    }
    taskCounter++;
}

void GameStepAttack(GameState& game, const std::unordered_map<int, DiscreteAction>& action, int side, int enemySide, unordered_map<int, int>& unitDamageMap, const unordered_map<Coord, int, UHasher<Coord>>& coordIdxMap) {
    for (const auto& [idx, act]: action) {
        if (game.objMap.count(idx) == 0) {
            continue;
        }
        auto& obj = game.objMap.at(idx);
        if (obj.owner != side) {
            continue;
        }
        if (act.action != ATTACK) {
            continue;
        }
        if (!obj.actionMask.canAttack) {
            continue;
        }
        auto iter = coordIdxMap.find(act.target);
        if (iter == coordIdxMap.end()) {
            continue;
        }
        auto targetIdx = iter->second;
        auto& target = game.objMap.at(targetIdx);
        if (!target.actionMask.canBeAttacked) {
            continue;
        }
        if (target.owner != enemySide) {
            continue;
        }
        if (obj.attackCD != 0) {
            continue;
        }
        if (obj.actionProgress != 0) {
            continue;
        }
        if (act.target.y < 0 || act.target.x < 0 || act.target.x >= game.w || act.target.y >= game.w) {
            continue;
        }
        if (obj.attackRange < sqrt(
                (act.target.y - obj.coord.y) * (act.target.y - obj.coord.y) +
                (act.target.x - obj.coord.x) * (act.target.x - obj.coord.x))) {
            continue;
        }
        obj.currentAction = act.action;
        obj.actionTarget = act.target;
        obj.attackCD = obj.attackInterval;
        obj.currentAction = NOOP;
        unitDamageMap[targetIdx] += obj.attackPoint;
    }
}

void GameExecuteAttack(GameState& game, const unordered_map<int, int>& unitDamageMap) {
    for (const auto& [idx, damage]: unitDamageMap) {
        auto iter = game.objMap.find(idx);
        if (iter == game.objMap.end()) {
            continue;
        }
        auto& obj = iter->second;
        obj.hitPoint -= damage;
        if (obj.hitPoint <= 0) {
            if (obj.type == BASE || obj.type == BARRACK) {
                auto player = obj.owner == -1 ? 0:1;
                game.buildingCnt[player]--;
            }
            game.objMap.erase(idx);
        }
    }
}

void GameStepProduce(GameState& game, std::unordered_map<int, DiscreteAction>& action, int side, unordered_map<Coord, int, UHasher<Coord>>& coordOccupationCount) {
    for (auto& [idx, act]: action) {
        if (act.action != PRODUCE) {
            continue;
        }
        act.action = NOOP;
        if (game.objMap.count(idx) == 0) {
            continue;
        }
        auto& obj = game.objMap.at(idx);
        if (obj.owner != side) {
            continue;
        }
        if (std::count(OBJ_PRODUCE_MAP.at(obj.type).begin(), OBJ_PRODUCE_MAP.at(obj.type).end(),act.produceType) == 0) {
            continue;
        }
        if (obj.attackCD != 0) {
            continue;
        }
        if (obj.actionProgress != 0) {
            continue;
        }
        if (act.target.y < 0 || act.target.x < 0 || act.target.x >= game.w || act.target.y >= game.w) {
            continue;
        }
        if (abs(act.target.x - obj.coord.x) + abs(act.target.y - obj.coord.y) > 1) {
            continue;
        }
        auto cost = OBJ_COST_MAP.at(act.produceType);
        auto playerIdx= side == -1 ? 0 : 1;
        if (game.resource[playerIdx] < cost) {
            continue;
        }
        act.action = PRODUCE;
        coordOccupationCount[act.target]++;
    }
}

void GameExecuteProduce(GameState& game, std::unordered_map<int, DiscreteAction>& action, int side, unordered_map<Coord, int, UHasher<Coord>>& coordOccupationCount) {
    for (auto& [idx, act]: action) {
        if (act.action != PRODUCE) {
            continue;
        }
        act.action = NOOP;
        if (game.objMap.count(idx) == 0) {
            continue;
        }
        auto& obj = game.objMap.at(idx);
        auto cost = OBJ_COST_MAP.at(act.produceType);
        auto playerIdx= side == -1 ? 0 : 1;
        if (game.resource[playerIdx] < cost) {
            continue;
        }
        if (coordOccupationCount.at(obj.coord) > 1) {
            continue;
        }
        act.action = PRODUCE;
        game.resource[playerIdx] -= cost;
        obj.currentAction = act.action;
        obj.actionTarget = act.target;
        obj.actionProgress = OBJ_TIME_MAP.at(act.produceType);
        obj.actionTotalProgress = obj.actionProgress;
        obj.produceType = act.produceType;
    }
}

void GameSettleProduce(GameState& game) {
    unordered_set<GameObj, UHasher<Coord>> newObjSet;
    for (auto& [_, obj]: game.objMap) {
        if (obj.currentAction != PRODUCE) {
            continue;
        }
        if (obj.actionProgress-- == 0) {
            obj.currentAction = NOOP;

            auto newObj = GameObj();
            newObj.coord = obj.actionTarget;
            newObj.actionTarget = newObj.coord;
            newObj.type = obj.produceType;
            newObj.owner = obj.owner;
            newObj.currentAction = NOOP;
            newObj.attackCD = 0;
            newObj.attackRange = OBJ_ATTACK_RANGE_MAP.count(newObj.type) ? OBJ_ATTACK_RANGE_MAP.at(newObj.type) : 0;
            newObj.attackInterval = OBJ_ATTACK_INTERVAL_MAP.count(newObj.type) ? OBJ_ATTACK_INTERVAL_MAP.at(newObj.type) : 0;
            newObj.attackPoint = OBJ_ATTACK_MAP.count(newObj.type) ? OBJ_ATTACK_MAP.at(newObj.type) : 0;
            newObj.actionMask = OBJ_ACTION_MASK_MAP.at(newObj.type);
            newObj.hitPoint = OBJ_HP_MAP.at(newObj.type);
            game.objMap.emplace(game.objCnt++, newObj);
            if (newObj.type == BASE || newObj.type == BARRACK) {
                auto player = newObj.owner == -1 ? 0:1;
                game.buildingCnt[player]++;
            }
        }
    }
}
void DumpAction(GameState& game, const TotalDiscreteAction& action) {
    cout << "time=" << game.time << endl;
    for (int i = 0; i < 2; ++i) {
        const auto& act = action.action[i];
        cout << "Player" << (char)('A' + i) << " :";
        for (const auto& [idx, a]: act) {
            Coord coord {-1, -1};
            if (game.objMap.count(idx)) {
                const auto& c = game.objMap.at(idx);
                coord = {c.coord.y, c.coord.x};
            }
            cout << "unit (" << coord.y << "," << coord.x <<"): ";
            switch (a.action) {
                case ATTACK:
                    cout << "ATTACK: ";
                    break;
                case MOVE:
                    cout << "MOVE: ";
                    break;
                case PRODUCE:
                    cout << "PRODUCE: " << a.produceType << " ";
                    break;
                case GATHER:
                    cout << "GATHER: ";
                    break;
                case RETURN:
                    cout << "RETURN: ";
                    break;
                default:
                    break;
            }
            cout << "(" << a.target.y << "," << a.target.x <<")" << endl;
        }
        cout << endl;
    }
}

void GameRefresh(GameState& game, bool enableLog) {
    for (auto& [_, obj]: game.objMap) {
        if (obj.currentAction == NOOP && obj.attackCD == 0) {
            obj.actionTarget = obj.coord;
            obj.actionProgress = 0;
            obj.actionTotalProgress = 0;
            obj.produceType = TERRAIN;
        }
        if (obj.type == BASE) {
            obj.resource = game.resource[obj.owner == -1 ? 0 : 1];
        }
        obj.attackCD = max(0, obj.attackCD - 1);
    }
    // continue game
    if (game.buildingCnt[0] != 0 && game.buildingCnt[1] != 0) {
        game.time++;
        return;
    }
    // end of game
    unordered_set<int> deleteSet;
    for (const auto& [idx, obj]: game.objMap) {
        if (obj.owner == 0) {
            continue;
        }
        auto side = obj.owner == -1 ? 0 : 1;
        if (game.buildingCnt[side] <= 0) {
            deleteSet.emplace(idx);
        }
    }
    for (const auto& idx: deleteSet) {
        game.objMap.erase(idx);
    }
    if (!enableLog) {
        return;
    }
    if (game.buildingCnt[0] == 0 && game.buildingCnt[1] == 1) {
        cout << "Game End: Draw" << endl;
    } else if (game.buildingCnt[1] == 0) {
        cout << "Game End: Player A Win" << endl;
    } else if (game.buildingCnt[0] == 0) {
        cout << "Game End: Player B Win" << endl;
    }
}

void GameStepMove(GameState& game, std::unordered_map<int, DiscreteAction>& action, int side, unordered_map<Coord, int, UHasher<Coord>>& coordOccupationCount) {
    for (auto& [idx, act]: action) {
        if (act.action != MOVE) {
            continue;
        }
        act.action = NOOP;
        if (game.objMap.count(idx) == 0) {
            continue;
        }
        auto& obj = game.objMap.at(idx);
        if (obj.owner != side) {
            continue;
        }
        if (act.target.y < 0 || act.target.x < 0 || act.target.x >= game.w || act.target.y >= game.w) {
            continue;
        }
        if (abs(act.target.x - obj.coord.x) + abs(act.target.y - obj.coord.y) > 1) {
            continue;
        }
        if (obj.attackCD != 0) {
            continue;
        }
        if (obj.actionProgress != 0) {
            continue;
        }
        if (!obj.actionMask.canMove) {
            continue;
        }
        act.action = MOVE;
        coordOccupationCount[act.target]++;
    }
}

void GameExecuteMove(GameState& game, std::unordered_map<int, DiscreteAction>& action, int side, unordered_map<Coord, int, UHasher<Coord>>& coordOccupationCount) {
    for (auto& [idx, act]: action) {
        if (act.action != MOVE) {
            continue;
        }
        act.action = NOOP;
        if (game.objMap.count(idx) == 0) {
            continue;
        }
        auto& obj = game.objMap.at(idx);
        if (coordOccupationCount.at(act.target) > 1) {
            continue;
        }
        act.action = MOVE;
        obj.currentAction = act.action;
        obj.actionTarget = act.target;
        obj.actionProgress = OBJ_MOVE_INTERVAL_MAP.at(obj.type);
        obj.actionTotalProgress = obj.actionProgress;
    }
}

void GameSettleMove(GameState& game) {
    unordered_set<GameObj, UHasher<Coord>> newObjSet;
    for (auto& [_, obj]: game.objMap) {
        if (obj.currentAction != MOVE) {
            continue;
        }
        if (obj.actionProgress-- == 0) {
            obj.currentAction = NOOP;
            obj.coord = obj.actionTarget;
        }
    }
}

void GameStepGather(GameState& game, std::unordered_map<int, DiscreteAction>& action, int side, unordered_map<Coord, int, UHasher<Coord>>& coordGatherCount, const unordered_map<Coord, int, UHasher<Coord>>& coordIdxMap) {
    for (auto& [idx, act]: action) {
        if (act.action != GATHER) {
            continue;
        }
        act.action = NOOP;
        if (game.objMap.count(idx) == 0) {
            continue;
        }
        auto& obj = game.objMap.at(idx);
        if (obj.owner != side) {
            continue;
        }
        if (obj.attackCD != 0) {
            continue;
        }
        if (obj.actionProgress != 0) {
            continue;
        }
        if (!obj.actionMask.canGather) {
            continue;
        }
        if (obj.resource != 0) {
            continue;
        }
        if (coordIdxMap.count(act.target) == 0) {
            continue;
        }
        if (abs(act.target.x - obj.coord.x) + abs(act.target.y - obj.coord.y) > 1) {
            continue;
        }
        auto& target = game.objMap.at(coordIdxMap.at(act.target));
        if (!target.actionMask.canBeGathered) {
            continue;
        }
        act.action = GATHER;
        coordGatherCount[act.target]++;
    }
}

void GameExecuteGather(GameState& game, std::unordered_map<int, DiscreteAction>& action, int side, unordered_map<Coord, int, UHasher<Coord>>& coordGatherCount) {
    for (auto& [idx, act]: action) {
        if (act.action != GATHER) {
            continue;
        }
        act.action = NOOP;
        if (game.objMap.count(idx) == 0) {
            continue;
        }
        auto& obj = game.objMap.at(idx);
        if (coordGatherCount.count(act.target) == 0 || coordGatherCount.at(act.target) > 1) {
            continue;
        }
        act.action = GATHER;
        obj.currentAction = GATHER;
        obj.actionTotalProgress = GATHER_TIME;
        obj.actionProgress = GATHER_TIME;
        obj.actionTarget = act.target;
    }
}

void GameSettleGather(GameState& game, const unordered_map<Coord, int, UHasher<Coord>>& coordIdxMap) {
    unordered_set<int> eraseSet;
    for (auto& [_, obj]: game.objMap) {
        if (obj.currentAction != GATHER) {
            continue;
        }
        auto& target = game.objMap.at(coordIdxMap.at(obj.actionTarget));
        if (obj.actionProgress-- == 0) {
            obj.currentAction = NOOP;
            obj.resource = min<int>(RES_PER_GATHER, target.resource);
            target.resource -= RES_PER_GATHER;
            if (target.resource <= 0) {
                eraseSet.emplace(coordIdxMap.at(obj.actionTarget));
            }
        }
    }
    for (const auto& idx: eraseSet) {
        game.objMap.erase(idx);
    }
}

void GameStepReturn(GameState& game, std::unordered_map<int, DiscreteAction>& action, int side, const unordered_map<Coord, int, UHasher<Coord>>& coordIdxMap) {
    for (auto& [idx, act]: action) {
        if (act.action != RETURN) {
            continue;
        }
        act.action = NOOP;
        if (game.objMap.count(idx) == 0) {
            continue;
        }
        auto& obj = game.objMap.at(idx);
        if (obj.owner != side) {
            continue;
        }
        if (obj.attackCD != 0) {
            continue;
        }
        if (obj.actionProgress != 0) {
            continue;
        }
        if (!obj.actionMask.canGather) {
            continue;
        }
        if (obj.resource == 0) {
            continue;
        }
        if (coordIdxMap.count(act.target) == 0) {
            continue;
        }
        if (abs(act.target.x - obj.coord.x) + abs(act.target.y - obj.coord.y) > 1) {
            continue;
        }
        auto& target = game.objMap.at(coordIdxMap.at(act.target));
        if (!target.actionMask.canBeStored) {
            continue;
        }
        if (side != target.owner) {
            continue;
        }
        game.resource[target.owner == -1 ? 0 : 1] += obj.resource;
        obj.resource = 0;
        act.action = NOOP;
    }
}


void GameStepSingle(GameState& game, TotalDiscreteAction& action, bool enableLog) {
    if (enableLog) {
        DumpAction(game, action);
    }
    unordered_map<Coord, int, UHasher<Coord>> coordIdxMap;
    for (auto& [idx, obj]: game.objMap) {
        coordIdxMap.emplace(obj.coord, idx);
    }
    unordered_map<int, int> unitDamageMap;
    GameStepAttack(game, action.action[0], -1, 1, unitDamageMap, coordIdxMap);
    GameStepAttack(game, action.action[1], 1, -1, unitDamageMap, coordIdxMap);
    GameExecuteAttack(game, unitDamageMap);
    unordered_map<Coord, int, UHasher<Coord>> coordOccupationCount;
    for (const auto& [_, obj]: game.objMap) {
        coordOccupationCount[obj.coord]++;
        if (obj.currentAction == MOVE || obj.currentAction == PRODUCE) {
            coordOccupationCount[obj.actionTarget]++;
        }
    }
    GameStepProduce(game, action.action[0], -1, coordOccupationCount);
    GameStepProduce(game, action.action[1], 1, coordOccupationCount);
    GameExecuteProduce(game, action.action[0], -1, coordOccupationCount);
    GameExecuteProduce(game, action.action[1], 1, coordOccupationCount);
    GameSettleProduce(game);

    GameStepMove(game, action.action[0], -1, coordOccupationCount);
    GameStepMove(game, action.action[1], 1, coordOccupationCount);
    GameExecuteMove(game, action.action[0], -1, coordOccupationCount);
    GameExecuteMove(game, action.action[1], 1, coordOccupationCount);
    GameSettleMove(game);

    unordered_map<Coord, int, UHasher<Coord>> coordGatherCount;
    for (const auto& [_, obj]: game.objMap) {
        if (obj.currentAction == GATHER) {
            coordGatherCount[obj.actionTarget]++;
        }
    }
    GameStepGather(game, action.action[0], -1, coordGatherCount, coordIdxMap);
    GameStepGather(game, action.action[1], 1, coordGatherCount, coordIdxMap);
    GameExecuteGather(game, action.action[0], -1, coordGatherCount);
    GameExecuteGather(game, action.action[1], 1, coordGatherCount);
    GameSettleGather(game, coordIdxMap);

    GameStepReturn(game, action.action[0], -1, coordIdxMap);
    GameStepReturn(game, action.action[1], 1, coordIdxMap);

    GameRefresh(game, enableLog);
    action.action[0].clear();
    action.action[1].clear();
}

#include "client_interface.h"
void GameStep(GameState *ptrGameState, signed char** actionDataArr, int* sizeArr, int idx, std::atomic<int> *ptrCounter) {
    auto& game = *ptrGameState;
    auto& counter = *ptrCounter;
    TotalDiscreteAction totalDiscreteAction;
    auto startIdx = ACTION_PLANE_NUM * game.w * game.h * idx;
    auto actionSize = ACTION_PLANE_NUM * game.w * game.h;
    if (startIdx + actionSize > sizeArr[0] || startIdx + actionSize > sizeArr[1]) {
        cerr << "Expect action size " << actionSize << " bytes is greater than " << sizeArr[0] << "/" << sizeArr[1] << "bytes" << endl;
        counter++;
        return;
    }

    unordered_map<Coord, int, UHasher<Coord>> coordIdxMap;
    for (const auto& [i, obj]: game.objMap) {
        coordIdxMap[obj.coord] = i;
    }
    for (int p = 0; p < 2; ++p) {
        for (int h = 0; h < game.h; ++h) {
            for (int w = 0; w < game.w; ++w) {
                if (coordIdxMap.count({h, w}) == 0) {
                    continue;
                }
                auto objIdx = coordIdxMap.at({h,w});
                totalDiscreteAction.action[p][objIdx] = {};
                auto& act = totalDiscreteAction.action[p].at(objIdx);
                act.action = static_cast<ActionType>(actionDataArr[p][startIdx + ACTION * game.w * game.h + h * game.w + w]);
                Coord dir;
                int attackDir;
                switch (act.action) {
                    case MOVE:
                        dir = DIRECTION_TARGET_MAP[actionDataArr[p][startIdx + MOVE_PARAM * game.w * game.h + h * game.w + w]];
                        act.produceType = TERRAIN;
                        act.target = {h + dir.y, w + dir.x};
                        break;
                    case GATHER:
                        dir = DIRECTION_TARGET_MAP[actionDataArr[p][startIdx + GATHER_PARAM * game.w * game.h + h * game.w + w]];
                        act.produceType = TERRAIN;
                        act.target = {h + dir.y, w + dir.x};
                        break;
                    case RETURN:
                        dir = DIRECTION_TARGET_MAP[actionDataArr[p][startIdx + RETURN_PARAM * game.w * game.h + h * game.w + w]];
                        act.produceType = TERRAIN;
                        act.target = {h + dir.y, w + dir.x};
                        break;
                    case PRODUCE:
                        dir = DIRECTION_TARGET_MAP[actionDataArr[p][startIdx + PRODUCE_DIRECTION_PARAM * game.w * game.h + h * game.w + w]];
                        act.produceType = static_cast<GameObjType>(actionDataArr[p][
                                PRODUCE_TYPE_PARAM * game.w * game.h + h * game.w + w]);
                        act.target = {h + dir.y, w + dir.x};
                        break;
                    case ATTACK:
                        attackDir = static_cast<int>(actionDataArr[p][startIdx + RELATIVE_ATTACK_POSITION * game.w * game.h + h * game.w + w]);
                        dir = {attackDir / (MAX_ATTACK_RANGE * 2 + 1) - MAX_ATTACK_RANGE, attackDir % (MAX_ATTACK_RANGE * 2 + 1) - MAX_ATTACK_RANGE};
                        act.produceType = TERRAIN;
                        act.target = {h + dir.y, w + dir.x};
                        break;
                    default:
                        break;
                }
            }
        }
    }
    GameStepSingle(*ptrGameState, totalDiscreteAction, false);
    counter++;
}