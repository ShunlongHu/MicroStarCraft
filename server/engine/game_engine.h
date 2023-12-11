//
// Created by zoe50 on 2023/12/11.
//

#ifndef RTS_GAME_ENGINE_H
#define RTS_GAME_ENGINE_H

#include <atomic>
#include "game_types.h"

void GameReset(GameState *ptrGameState, int seed, bool isRotSym, bool isAxSym, double terrainProb, int expansionCnt,
               int clusterPerExpansion, int mineralPerCluster, std::atomic<int> *ptrCounter);

void GameStep(GameState *ptrGameState, std::atomic<int> *ptrCounter);

#endif //RTS_GAME_ENGINE_H
