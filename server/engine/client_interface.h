//
// Created by zoe50 on 2023/12/9.
//

#ifndef RTS_CLIENT_INTERFACE_H
#define RTS_CLIENT_INTERFACE_H

#include "game_types.h"

const int W = 32;
const int H = 32;

struct InitParam {
    int w;
    int h;
    int numWorkers;
};

extern "C" __declspec(dllexport) void Init(InitParam initParam);
extern "C" __declspec(dllexport) TotalObservation
Reset(int seed, bool isRotSym, bool isAxSym, double terrainProb, int expansionCnt, int clusterPerExpansion,
      int mineralPerCluster);
extern "C" __declspec(dllexport) TotalObservation Step(TotalAction action);
extern "C" __declspec(dllexport) void Render(int gameIdx);

GameState &GetGameState(int gameIdx);


#endif //RTS_CLIENT_INTERFACE_H
