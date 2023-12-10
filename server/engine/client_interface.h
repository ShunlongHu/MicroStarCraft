//
// Created by zoe50 on 2023/12/9.
//

#ifndef RTS_CLIENT_INTERFACE_H
#define RTS_CLIENT_INTERFACE_H
#include "game_types.h"

struct InitParam {
    int w;
    int h;
    int numWorkers;
};

enum ObservationPlane {
    HP_0,
    HP_1,
    HP_2,
    HP_3,
    HP_4PLUS,
    RES_0,
    RES_1,
    RES_2,
    RES_3,
    RES_4PLUS,
    OWNER_1,
    OWNER_NONE,
    OWNER_2,
    OBJ_TYPE,
    CURRENT_ACTION = OBJ_TYPE + GAME_OBJ_TYPE_NUM,
};

constexpr static int OBSERVATION_PLANE_NUM = CURRENT_ACTION + GAME_ACTION_TYPE_NUM + 1;

struct Observation {
    signed char* data = nullptr;
    int size = 0;

    int newWorkerCnt = 0;
    int newLightCnt = 0;
    int newRangedCnt = 0;
    int newHeavyCnt = 0;
    int newBaseCnt = 0;
    int newBarrackCnt = 0;

    int deadWorkerCnt = 0;
    int deadLightCnt = 0;
    int deadRangedCnt = 0;
    int deadHeavyCnt = 0;
    int deadBaseCnt = 0;
    int deadBarrackCnt = 0;

    int newWorkerKilled = 0;
    int newLightKilled = 0;
    int newRangedKilled = 0;
    int newHeavyKilled = 0;
    int newBaseKilled = 0;
    int newBarrackKilled = 0;

    int newNetIncome = 0;
    int newHitCnt = 0;
};

enum ActionPlane {
    ACTION,
    MOVE_PARAM,
    GATHER_PARAM,
    RETURN_PARAM,
    PRODUCE_DIRECTION_PARAM,
    PRODUCE_TYPE_PARAM,
    RELATIVE_ATTACK_POSITION
};

constexpr static int ACTION_PLANE_NUM = RELATIVE_ATTACK_POSITION + 1;

struct Action {
    signed char* data;
    int size;
};

struct TotalObservation {
    Observation ob1;
    Observation ob2;
    int time = 0;
    bool isEnd = false;
    int winningSide = 0;
};

struct TotalAction {
    Action action1;
    Action action2;
};

extern "C" __declspec(dllexport) void Init(InitParam initParam);
extern "C" __declspec(dllexport) TotalObservation Reset(int seed, bool isRotSym, bool isAxSym, double terrainProb, int expansionCnt, int clusterPerExpansion, int mineralPerCluster);
extern "C" __declspec(dllexport) TotalObservation Step(TotalAction action);
extern "C" __declspec(dllexport) void Render(int gameIdx);

GameState& GetGameState(int gameIdx);


#endif //RTS_CLIENT_INTERFACE_H
