//
// Created by zoe50 on 2023/12/9.
//

#ifndef RTS_CLIENT_INTERFACE_H
#define RTS_CLIENT_INTERFACE_H

#include "game_types.h"

const int W = 32;
const int H = 32;


enum ObservationPlane {
    HP_1,
    HP_2,
    HP_3,
    HP_4,
    HP_5,
    HP_6_PLUS,
    RES_1,
    RES_2,
    RES_3,
    RES_4,
    RES_5,
    RES_6_PLUS,
    OWNER_1,
    OWNER_NONE,
    OWNER_2,
    OBSTACLE,
    GATHERING,
    OBJ_TYPE,
    CURRENT_ACTION = OBJ_TYPE + GAME_OBJ_TYPE_NUM,
};

constexpr static int OBSERVATION_PLANE_NUM = CURRENT_ACTION + GAME_ACTION_TYPE_NUM;

enum Reward {
    GAME_TIME,
    IS_END,
    VICTORY_SIDE,

    NEW_WORKER_CNT,
    NEW_LIGHT_CNT,
    NEW_RANGED_CNT,
    NEW_HEAVY_CNT,
    NEW_BASE_CNT,
    NEW_BARRACK_CNT,

    DEAD_WORKER_CNT,
    DEAD_LIGHT_CNT,
    DEAD_RANGED_CNT,
    DEAD_HEAVY_CNT,
    DEAD_BASE_CNT,
    DEAD_BARRACK_CNT,

    NEW_WORKER_KILLED,
    NEW_LIGHT_KILLED,
    NEW_RANGED_KILLED,
    NEW_HEAVY_KILLED,
    NEW_BASE_KILLED,
    NEW_BARRACK_KILLED,

    NEW_NET_INCOME,
    NEW_HIT_CNT,
};

constexpr int GAME_STAT_NUM = NEW_HIT_CNT + 1;

struct Observation {
    signed char *data = nullptr;
    int size = 0;
    int *reward = nullptr;
    int rewardSize = 0;
    signed char* mask = nullptr;
    int maskSize = 0;
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

enum AIActionMask {
    ACTION_TYPE_MASK = 0,
    MOVE_PARAM_MASK = ACTION_TYPE_MASK + GAME_ACTION_TYPE_NUM,
    GATHER_PARAM_MASK = MOVE_PARAM_MASK + 4,
    RETURN_PARAM_MASK = GATHER_PARAM_MASK + 4,
    PRODUCE_DIRECTION_PARAM_MASK = RETURN_PARAM_MASK + 4,
    PRODUCE_TYPE_PARAM_MASK = PRODUCE_DIRECTION_PARAM_MASK + 4,
    RELATIVE_ATTACK_POSITION_MASK = PRODUCE_TYPE_PARAM_MASK + GAME_PRODUCIBLE_OBJ_TYPE_NUM
};
constexpr static int ACTION_MASK_SIZE = RELATIVE_ATTACK_POSITION_MASK + 49;

struct Action {
    signed char *data;
    int size;
};

struct TotalObservation {
    Observation ob1;
    Observation ob2;
};

struct TotalAction {
    Action action1;
    Action action2;
};

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
void Step(TotalDiscreteAction& action);

GameState &GetGameState(int gameIdx);


#endif //RTS_CLIENT_INTERFACE_H
