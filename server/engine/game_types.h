//
// Created by zoe50 on 2023/12/10.
//

#ifndef RTS_GAME_TYPES_H
#define RTS_GAME_TYPES_H
#include <unordered_map>
#include <vector>

// GAME PARAMS
constexpr static int GAME_OBJ_TYPE_NUM = 8;
constexpr static int GAME_PRODUCIBLE_OBJ_TYPE_NUM = 6;
constexpr static int GAME_ACTION_TYPE_NUM = 6;
constexpr static int GAME_DIRECTION_TYPE_NUM = 4;

// GAME_ATTACK
constexpr static int RADIUS_MELEE = 1;
constexpr static int RADIUS_RANGED = 3;
constexpr static int HP_BASE = 60; // starcraft building hp/25
constexpr static int HP_BARRACK = 40;
constexpr static int HP_WORKER = 2; // starcraft unit hp/25
constexpr static int HP_LIGHT = 2;
constexpr static int HP_HEAVY = 6;
constexpr static int HP_RANGED = 2;
constexpr static int ATTACK_WORKER = 1;
constexpr static int ATTACK_LIGHT = 2;
constexpr static int ATTACK_HEAVY = 2;
constexpr static int ATTACK_RANGED = 1;

// GAME_ECONOMY
constexpr static int RES_PER_GATHER = 1;
constexpr static int COST_BASE = 4;
constexpr static int COST_BARRACK = 3;
constexpr static int COST_WORKER = 2;
constexpr static int COST_LIGHT = 1;
constexpr static int COST_HEAVY = 4;
constexpr static int COST_RANGED = 2;
constexpr static int TIME_BASE = 36;
constexpr static int TIME_BARRACK = 24;
constexpr static int TIME_WORKER = 6; // scv
constexpr static int TIME_LIGHT = 4;  // zergling
constexpr static int TIME_HEAVY = 12;   // zelot
constexpr static int TIME_RANGED = 9;  // marine

template <class T> struct UHasher{
    size_t operator() (const T& key) const {
        if (sizeof(T) == 8) {
            return *reinterpret_cast<const uint64_t*>(&key);
        }
        if (sizeof(T) == 4) {
            return *reinterpret_cast<const uint32_t*>(&key);
        }
        if (sizeof(T) == 2) {
            return *reinterpret_cast<const uint16_t*>(&key);
        }
        if (sizeof(T) == 1) {
            return *reinterpret_cast<const uint8_t*>(&key);
        }
        return std::_Hash_array_representation(reinterpret_cast<const char*>(&key), sizeof(key));
    }
};

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

// (0, 0) = TOP LEFT
enum DirectionType {
    NORTH,
    EAST,
    SOUTH,
    WEST
};

struct Coord {
    int y;
    int x;
    bool operator== (const Coord& second) const {
        return second.x == x && second.y == y;
    }
};
using ActionTarget = Coord;

const static std::vector<ActionTarget> DIRECTION_TARGET_MAP {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};
const static  std::vector<ActionTarget> ATTACK_RANGE_TARGET_MAP {
        {-3, -3},
        {-3, -2},
        {-3, -1},
        {-3, -0},
        {-3, 1},
        {-3, 2},
        {-3, 3},

        {-2, -3},
        {-2, -2},
        {-2, -1},
        {-2, -0},
        {-2, 1},
        {-2, 2},
        {-2, 3},

        {-1, -3},
        {-1, -2},
        {-1, -1},
        {-1, -0},
        {-1, 1},
        {-1, 2},
        {-1, 3},

        {0, -3},
        {0, -2},
        {0, -1},
        {0, -0},
        {0, 1},
        {0, 2},
        {0, 3},

        {1, -3},
        {1, -2},
        {1, -1},
        {1, -0},
        {1, 1},
        {1, 2},
        {1, 3},

        {2, -3},
        {2, -2},
        {2, -1},
        {2, -0},
        {2, 1},
        {2, 2},
        {2, 3},

        {3, -3},
        {3, -2},
        {3, -1},
        {3, -0},
        {3, 1},
        {3, 2},
        {3, 3},
};

struct GameObj {
    GameObjType type;
    int hitPoint;
    int resource;
    int owner;
    ActionType currentAction;
    ActionTarget actionTarget;
    int actionProgress;
    int actionTotalProgress;
};

struct GameState {
    std::unordered_map<Coord, GameObj, UHasher<Coord>> objMap;
    int w;
    int h;
    int time;
};

#endif //RTS_GAME_TYPES_H
