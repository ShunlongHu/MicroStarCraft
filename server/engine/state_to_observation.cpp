//
// Created by zoe50 on 2023/12/11.
//

#include "state_to_observation.h"
#include <sstream>
using namespace std;
void StateToObservation(const GameState* ptrGameState, const GameState* ptrLastGameState, std::vector<signed char>* observationVec, std::vector<int>* rewardVec, int idx, std::atomic<int>* ptrCounter) {
    auto& counter = *ptrCounter;
    const auto& game = *ptrGameState;
    const auto& lastGame = *ptrLastGameState;
    auto observationStartPos = idx * OBSERVATION_PLANE_NUM * game.w * game.h;
    auto rewardStartPos = idx * GAME_STAT_NUM;
    signed char* ob[2] {observationVec[0].data() + observationStartPos, observationVec[1].data() + observationStartPos};
    signed int* re[2] {rewardVec[0].data() + rewardStartPos, rewardVec[1].data() + rewardStartPos};

    for (int i = 0; i < OBSERVATION_PLANE_NUM * game.w * game.h; ++i) {
        ob[0][i] = false;
    }
    for (int i = 0; i < GAME_STAT_NUM; ++i) {
        re[0][i] = 0;
        re[1][i] = 0;
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
    counter++;
}
