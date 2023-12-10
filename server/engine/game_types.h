//
// Created by zoe50 on 2023/12/10.
//

#ifndef RTS_GAME_TYPES_H
#define RTS_GAME_TYPES_H
#include <vector>

constexpr static int GAME_OBJ_TYPE_NUM = 8;
constexpr static int GAME_ACTION_TYPE_NUM = 6;

enum GameObjType {
    TERRAIN,
    MINERAL,
    BASE,
    BARRACK,
    WORKER,
    LIGHT,
    HEAVY,
    RANGED
};

enum ActionType {
    NOOP,
    MOVE,
    GATHER,
    RETURN,
    PRODUCE,
    ATTACK
};

struct Coord {
    int y;
    int x;
};

using ActionTarget = Coord;

struct GameObj {
    GameObjType type;
    Coord coord;
    int hitPoint;
    int resource;
    int owner;
    ActionType currentAction;
    ActionTarget actionTarget;
    int actionProgress;
    int actionTotalProgress;
};

struct GameState {
    std::vector<GameObj> objVec;
};

#endif //RTS_GAME_TYPES_H
