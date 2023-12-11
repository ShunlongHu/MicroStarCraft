//
// Created by zoe50 on 2023/12/11.
//

#include "state_to_observation.h"
using namespace std;
void StateToObservation(const GameState* ptrGameState, std::vector<signed char>* observationVec, std::vector<int>* rewardVec, int idx, std::atomic<int>* ptrCounter) {
    const auto& gameState = *ptrGameState;
    auto& counter = *ptrCounter;

    counter++;
}
