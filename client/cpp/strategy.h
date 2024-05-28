//
// Created by zoe50 on 2024/1/18.
//

#ifndef RTS_STRATEGY_H
#define RTS_STRATEGY_H
#include <unordered_map>
#include "game_types.h"

namespace Strategy {
void Act(const GameState& state, std::unordered_map<int, DiscreteAction>& action, int8_t side);
};


#endif //RTS_STRATEGY_H
