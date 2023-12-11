//
// Created by zoe50 on 2023/12/11.
//

#ifndef RTS_STATE_TO_OBSERVATION_H
#define RTS_STATE_TO_OBSERVATION_H
#include <atomic>
#include <vector>
#include "game_types.h"
#include "client_interface.h"

void StateToObservation(const GameState* ptrGameState, std::vector<signed char>* observationVec, std::vector<int>* rewardVec, int idx, std::atomic<int>* ptrCounter);


#endif //RTS_STATE_TO_OBSERVATION_H
