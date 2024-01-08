//
// Created by zoe50 on 2023/12/11.
//

#include "state_to_observation.h"
using namespace std;
void StateToObservation(const GameState* ptrGameState, std::vector<signed char>* observationVec, std::vector<int>* rewardVec, int idx, std::atomic<int>* ptrCounter) {
    const auto& game = *ptrGameState;
    auto& counter = *ptrCounter;
    auto observationStartPos = idx * OBSERVATION_PLANE_NUM * game.w * game.h;
    auto rewardStartPos = idx * GAME_STAT_NUM * game.w * game.h;
    signed char* ob[2] {observationVec[0].data() + observationStartPos, observationVec[1].data() + observationStartPos};
    signed int* re[2] {rewardVec[0].data() + rewardStartPos, rewardVec[1].data() + rewardStartPos};

    for (int i = 0; i < OBSERVATION_PLANE_NUM * game.w * game.h; ++i) {
        ob[0][i] = false;
    }

    for (const auto& [_, obj]: game.objMap) {
        auto coord = obj.coord.x + obj.coord.y * game.w;
        if (OBJ_HP_MAP.count(obj.type)) {
            auto layer = HP_1 + obj.hitPoint -1;
            layer = max<int>(layer, HP_6_PLUS);
            ob[0][layer * game.w * game.h + coord] = true;
        }
        if (obj.type == BASE || obj.type == WORKER || obj.type == MINERAL) {
            auto layer = RES_1 + obj.resource -1;
            layer = max<int>(layer, RES_6_PLUS);
            ob[0][layer * game.w * game.h + coord] = true;
        }
        ob[0][(OWNER_NONE + obj.owner) * game.w * game.h + coord] = true;
        ob[0][(OBJ_TYPE + obj.type) * game.w * game.h + coord] = true;
        ob[0][(CURRENT_ACTION + obj.currentAction) * game.w * game.h + coord] = true;
    }
    for (int i = 0; i < OBSERVATION_PLANE_NUM * game.w * game.h; ++i) {
        ob[1][i] = ob[0][i];
    }
    counter++;
}
