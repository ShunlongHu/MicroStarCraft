//
// Created by zoe50 on 2024/1/10.
//

#ifndef RTS_HUMAN_AI_H
#define RTS_HUMAN_AI_H
#include "rpc_client.h"

namespace HumanAi {
    void Act(const GameState& game, Coord mouseClick, Coord mouseRightClick);
};


#endif //RTS_HUMAN_AI_H
