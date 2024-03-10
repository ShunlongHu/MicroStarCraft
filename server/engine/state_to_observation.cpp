//
// Created by zoe50 on 2023/12/11.
//

#include "state_to_observation.h"
#include <sstream>
using namespace std;
void StateToObservation(const GameState* ptrGameState, const GameState* ptrLastGameState, std::vector<signed char>* observationVec, std::vector<int>* rewardVec, std::vector<signed char>* maskVec, int idx, std::atomic<int>* ptrCounter) {
    auto& counter = *ptrCounter;
    const auto& game = *ptrGameState;
    const auto& lastGame = *ptrLastGameState;
    auto observationStartPos = idx * OBSERVATION_PLANE_NUM * game.w * game.h;
    auto rewardStartPos = idx * GAME_STAT_NUM;
    auto maskStartPos = idx * game.w * game.h * ACTION_MASK_SIZE;
    signed char* ob[2] {observationVec[0].data() + observationStartPos, observationVec[1].data() + observationStartPos};
    signed int* re[2] {rewardVec[0].data() + rewardStartPos, rewardVec[1].data() + rewardStartPos};
    signed char* mask[2] {maskVec[0].data() + maskStartPos, maskVec[1].data() + maskStartPos};
    unordered_map<Coord, int> coordIdxMap;
    for (int i = 0; i < OBSERVATION_PLANE_NUM * game.w * game.h; ++i) {
        ob[0][i] = false;
    }
    for (int i = 0; i < GAME_STAT_NUM; ++i) {
        re[0][i] = 0;
        re[1][i] = 0;
    }
    for (int i = 0; i < ACTION_MASK_SIZE * game.w * game.h; ++i) {
        mask[0][i] = false;
        mask[1][i] = false;
    }

    for (const auto& [_, obj]: game.objMap) {
        auto coord = obj.coord.x + obj.coord.y * game.w;
        if (OBJ_HP_MAP.count(obj.type)) {
            auto layer = HP_1 + obj.hitPoint -1;
            layer = min<int>(layer, HP_6_PLUS);
            ob[0][layer * game.w * game.h + coord] = true;
        }
        if (obj.type == BASE || obj.type == WORKER || obj.type == MINERAL) {
            auto layer = RES_1 + obj.resource -1;
            layer = min<int>(layer, RES_6_PLUS);
            ob[0][layer * game.w * game.h + coord] = true;
        }
        ob[0][OBSTACLE * game.w * game.h + coord] = true;
        if (obj.currentAction == MOVE || obj.currentAction == PRODUCE) {
            auto target = obj.actionTarget.x + obj.actionTarget.y * game.w;
            ob[0][OBSTACLE * game.w * game.h + target] = true;
        } else if (obj.currentAction == GATHER) {
            auto target = obj.actionTarget.x + obj.actionTarget.y * game.w;
            ob[0][GATHERING * game.w * game.h + target] = true;
        }
        ob[0][(OWNER_NONE + obj.owner) * game.w * game.h + coord] = true;
        ob[0][(OBJ_TYPE + obj.type) * game.w * game.h + coord] = true;
        ob[0][(CURRENT_ACTION + obj.currentAction) * game.w * game.h + coord] = OBJ_TIME_MAP.count(obj.type) != 0;
    }
    for (int i = 0; i < OBSERVATION_PLANE_NUM * game.w * game.h; ++i) {
        ob[1][i] = ob[0][i];
    }


    if (game.buildingCnt[0] == 0 || game.buildingCnt[1] == 0) {
        re[0][VICTORY_SIDE] = game.buildingCnt[0] > 0 ? -1 : (game.buildingCnt[1] > 0 ? 1 : 0);
        re[1][VICTORY_SIDE] = game.buildingCnt[0] > 0 ? -1 : (game.buildingCnt[1] > 0 ? 1 : 0);
    }

    for (int p = 0; p < 2; ++p) {
        re[p][GAME_TIME] = game.time;
        re[p][IS_END] = (game.buildingCnt[0] == 0 || game.buildingCnt[1] == 0); // && (lastGame.buildingCnt[0] == 0 || lastGame.buildingCnt[1] == 0);
        if (game.buildingCnt[0] == 0 || game.buildingCnt[1] == 0) {
            re[p][VICTORY_SIDE] = game.buildingCnt[0] > 0 ? -1 : (game.buildingCnt[1] > 0 ? 1 : 0);
        }
        re[p][NEW_NET_INCOME] = game.resource[p] - lastGame.resource[p];
        auto side = p * 2 - 1;
        for (const auto& [unitIdx, obj]: game.objMap) {
            if (lastGame.objMap.count(unitIdx) != 0) {
                continue;
            }
            if (obj.owner != side) {
                continue;
            }
            switch (obj.type) {
                case WORKER:
                    re[p][NEW_WORKER_CNT]++;
                    break;
                case LIGHT:
                    re[p][NEW_LIGHT_CNT]++;
                    break;
                case RANGED:
                    re[p][NEW_RANGED_CNT]++;
                    break;
                case HEAVY:
                    re[p][NEW_HEAVY_CNT]++;
                    break;
                case BASE:
                    re[p][NEW_BASE_CNT]++;
                    break;
                case BARRACK:
                    re[p][NEW_BARRACK_CNT]++;
                    break;
                default:
                    break;
            }
        }
        for (const auto& [unitIdx, obj]: lastGame.objMap) {
            auto newHp = 0;
            if (obj.owner == side) {
                if (game.objMap.count(unitIdx) == 0) {
                    switch (obj.type) {
                        case WORKER:
                            re[p][DEAD_WORKER_CNT]++;
                            break;
                        case LIGHT:
                            re[p][DEAD_LIGHT_CNT]++;
                            break;
                        case RANGED:
                            re[p][DEAD_RANGED_CNT]++;
                            break;
                        case HEAVY:
                            re[p][DEAD_HEAVY_CNT]++;
                            break;
                        case BASE:
                            re[p][DEAD_BASE_CNT]++;
                            break;
                        case BARRACK:
                            re[p][DEAD_BARRACK_CNT]++;
                            break;
                        default:
                            break;
                    }
                }
            }
            if (obj.owner != -side) {
                continue;
            }
            if (game.objMap.count(unitIdx) != 0) {
                newHp = game.objMap.at(unitIdx).hitPoint;
            }
            re[p][NEW_HIT_CNT] += obj.hitPoint - newHp;
            if (game.objMap.count(unitIdx) != 0) {
                continue;
            }
            switch (obj.type) {
                case WORKER:
                    re[p][NEW_WORKER_KILLED]++;
                    break;
                case LIGHT:
                    re[p][NEW_LIGHT_KILLED]++;
                    break;
                case RANGED:
                    re[p][NEW_RANGED_KILLED]++;
                    break;
                case HEAVY:
                    re[p][NEW_HEAVY_KILLED]++;
                    break;
                case BASE:
                    re[p][NEW_BASE_KILLED]++;
                    break;
                case BARRACK:
                    re[p][NEW_BARRACK_KILLED]++;
                    break;
                default:
                    break;
            }
        }
    }
    // calculate action mask
    for (int p = 0; p < 2; ++p) {
        auto side = p * 2 - 1;
        // process noop
        for (int i = 0; i < game.w * game.h; ++i) {
            mask[p][(ACTION_TYPE_MASK + NOOP) * game.w * game.h + i] = true;
        }
        for (const auto& [unitIdx, obj]: game.objMap) {
            if (obj.owner != side) {
                continue;
            }
            if (obj.attackCD != 0 || obj.actionProgress != 0 || obj.currentAction != NOOP) {
                continue;
            }
            const auto& m = OBJ_ACTION_MASK_MAP.at(obj.type);
            for (int i = 0; i < DIRECTION_TARGET_MAP.size(); ++i) {
                auto target = Coord{obj.coord.y + DIRECTION_TARGET_MAP[i].y, obj.coord.x + DIRECTION_TARGET_MAP[i].x};
                if (target.y < 0 || target.x < 0 || target.x >= game.w || target.y >= game.w) {
                    continue;
                }
                auto targetCoord = target.y * game.w + target.x;
                // process move & produce
                if (!ob[0][OBSTACLE * game.w * game.h + targetCoord]) {
                    mask[p][(ACTION_TYPE_MASK + MOVE) * game.w * game.h + obj.coord.y * game.w + obj.coord.x] = m.canMove;
                    mask[p][(MOVE_PARAM_MASK + i) * game.w * game.h + obj.coord.y * game.w + obj.coord.x] = true;
                    if (OBJ_PRODUCE_MAP.count(obj.type) != 0) {
                        if (obj.type == BASE && game.resource[p] >= OBJ_COST_MAP.at(WORKER)) {
                            mask[p][(ACTION_TYPE_MASK + PRODUCE) * game.w * game.h + obj.coord.y * game.w + obj.coord.x] = true;
                            mask[p][(PRODUCE_DIRECTION_PARAM_MASK + i) * game.w * game.h + obj.coord.y * game.w + obj.coord.x] = true;
                            mask[p][(PRODUCE_TYPE_PARAM_MASK + WORKER - BASE) * game.w * game.h + obj.coord.y * game.w + obj.coord.x] = true;
                        }
                        if (obj.type == WORKER && game.resource[p] >= OBJ_COST_MAP.at(BARRACK)) {
                            mask[p][(ACTION_TYPE_MASK + PRODUCE) * game.w * game.h + obj.coord.y * game.w + obj.coord.x] = true;
                            mask[p][(PRODUCE_DIRECTION_PARAM_MASK + i) * game.w * game.h + obj.coord.y * game.w + obj.coord.x] = true;
                            mask[p][(PRODUCE_TYPE_PARAM_MASK + BARRACK - BASE) * game.w * game.h + obj.coord.y * game.w + obj.coord.x] = true;
                            mask[p][(PRODUCE_TYPE_PARAM_MASK + BASE - BASE) * game.w * game.h + obj.coord.y * game.w + obj.coord.x] = game.resource[p] >= OBJ_COST_MAP.at(BASE);
                        }
                        if (obj.type == BARRACK && game.resource[p] >= OBJ_COST_MAP.at(LIGHT)) {
                            mask[p][(ACTION_TYPE_MASK + PRODUCE) * game.w * game.h + obj.coord.y * game.w + obj.coord.x] = true;
                            mask[p][(PRODUCE_DIRECTION_PARAM_MASK + i) * game.w * game.h + obj.coord.y * game.w + obj.coord.x] = true;
                            mask[p][(PRODUCE_TYPE_PARAM_MASK + LIGHT - BASE) * game.w * game.h + obj.coord.y * game.w + obj.coord.x] = true;
                            mask[p][(PRODUCE_TYPE_PARAM_MASK + RANGED - BASE) * game.w * game.h + obj.coord.y * game.w + obj.coord.x] = game.resource[p] >= OBJ_COST_MAP.at(RANGED);
                            mask[p][(PRODUCE_TYPE_PARAM_MASK + HEAVY - BASE) * game.w * game.h + obj.coord.y * game.w + obj.coord.x] = game.resource[p] >= OBJ_COST_MAP.at(HEAVY);
                        }
                    }
                }
                // process gather
                if (m.canGather && obj.resource == 0 && ob[0][(OBJ_TYPE + MINERAL) * game.w * game.h + targetCoord]) {
                    mask[p][(ACTION_TYPE_MASK + GATHER) * game.w * game.h + obj.coord.y * game.w + obj.coord.x] = true;
                    mask[p][(GATHER_PARAM_MASK + i) * game.w * game.h + obj.coord.y * game.w + obj.coord.x] = true;
                }
                // process return
                if (m.canGather && obj.resource > 0 &&
                    ob[0][(OBJ_TYPE + BASE) * game.w * game.h + targetCoord] &&
                    ob[0][(OWNER_NONE + side) * game.w * game.h + targetCoord]) {
                    mask[p][(ACTION_TYPE_MASK + RETURN) * game.w * game.h + obj.coord.y * game.w + obj.coord.x] = true;
                    mask[p][(RETURN_PARAM_MASK + i) * game.w * game.h + obj.coord.y * game.w + obj.coord.x] = true;
                }
            }
            // process attack
            if (m.canAttack) {
                for (int i = 0; i < (MAX_ATTACK_RANGE * 2 + 1) * (MAX_ATTACK_RANGE * 2 + 1); ++i) {
                    auto dir = Coord{i / (MAX_ATTACK_RANGE * 2 + 1) - MAX_ATTACK_RANGE, i % (MAX_ATTACK_RANGE * 2 + 1) - MAX_ATTACK_RANGE};
                    if (dir.x * dir.x + dir.y * dir.y > obj.attackRange * obj.attackRange) {
                        continue;
                    }
                    auto target = Coord{obj.coord.y + dir.y, obj.coord.x + dir.x};
                    auto targetCoord = target.y * game.w + target.x;
                    if (ob[0][(OWNER_NONE - side) * game.w * game.h + targetCoord]) {
                        mask[p][(ACTION_TYPE_MASK + ATTACK) * game.w * game.h + obj.coord.y * game.w + obj.coord.x] = true;
                        mask[p][(RELATIVE_ATTACK_POSITION_MASK + i) * game.w * game.h + obj.coord.y * game.w + obj.coord.x] = true;
                    }
                }
            }
        }
    }
    counter++;
}
