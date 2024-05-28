//
// Created by zoe50 on 2024/1/18.
//

#include "Strategy.h"
#include <random>
using namespace std;
using namespace Strategy;
void Strategy::Act(const GameState& state, unordered_map<int, DiscreteAction>& action, int8_t side) {
    for (const auto& [idx, obj]: state.objMap) {
        if (obj.owner != side) {
            continue;
        }
        DiscreteAction act;
        act.action = static_cast<ActionType>(rand() % GAME_ACTION_TYPE_NUM);
        act.target = {rand() % state.w, rand() % state.h};
        act.produceType = static_cast<GameObjType>(rand() % GAME_OBJ_TYPE_NUM);
        action[idx] = act;
    }
}