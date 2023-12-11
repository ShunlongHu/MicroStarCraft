//
// Created by zoe50 on 2023/12/11.
//

#include "game_engine.h"
using namespace std;
void GameReset(GameState *ptrGameState, int seed, bool isRotSym, bool isAxSym, double terrainProb, int expansionCnt,
               int clusterPerExpansion, int mineralPerCluster, std::atomic<int> *ptrCounter) {
    auto& gameState = *ptrGameState;
    auto& counter = *ptrCounter;

    counter++;
}

void GameStep(GameState* ptrGameState, atomic<int>* ptrCounter) {
    auto& gameState = *ptrGameState;
    auto& counter = *ptrCounter;

    counter++;
}