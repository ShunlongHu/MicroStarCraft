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

const static std::unordered_map<GameObjType, int> OBJ_HP_MAP {  // sc2 / 25
    {BASE, 60},
    {BARRACK, 40},
    {WORKER, 4},
    {LIGHT, 4},
    {HEAVY, 12},
    {RANGED, 4}
};

const static std::unordered_map<GameObjType, int> OBJ_ATTACK_MAP { // sc2 / 40
    {WORKER, 1},
    {LIGHT, 1},
    {HEAVY, 4},
    {RANGED, 1}
};

const static std::unordered_map<GameObjType, int> OBJ_ATTACK_CD_MAP { // sc2 * 2
        {WORKER, 1},
        {LIGHT, 0},
        {HEAVY, 1},
        {RANGED, 0}
};

const static std::unordered_map<GameObjType, int> OBJ_COST_MAP { // sc2 / 50
        {BASE, 16},
        {BARRACK, 6},
        {WORKER, 2},
        {LIGHT, 1},
        {HEAVY, 4},
        {RANGED, 2}
};

const static std::unordered_map<GameObjType, int> OBJ_TIME_MAP { // sc2 / 4
        {BASE, 25},
        {BARRACK, 16},
        {WORKER, 4},
        {LIGHT, 3},
        {HEAVY, 7},
        {RANGED, 6}
};

const static std::unordered_map<GameObjType, int> OBJ_MOVE_INTERVAL_MAP { // (移动前摇) sc2 * 2
        {WORKER, 1},
        {LIGHT, 0},
        {HEAVY, 1},
        {RANGED, 1}
};

// GAME_ATTACK
constexpr static int RADIUS_MELEE = 1;
constexpr static int RADIUS_RANGED = 3;

// GAME_ECONOMY
constexpr static int RES_PER_GATHER = 1;
constexpr static int GATHER_POST = 0;

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

// 攻击无法选择方向，以默认攻击方向进行攻击
const static std::unordered_map<int, std::vector<Coord>> ATTACK_RANGE_COORD_MAP{
        /*
         *   x
         *  xox
         *   x
         */
        {1, {{-1, 0}, {1, 0}, {0, -1}, {0, 1}}},
        /*   xxx
         *  x o x
         *   xxx
         */
        {2, {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}, {-2, 0}, {2, 0}, {0, -2}, {0, 2}}},
        /*
         *    x
         *  xx xx
         *  x   x
         * x  o  x
         *  x   x
         *  xx xx
         *    x
         */
        {3, {{2, 1}, {2, -1}, {-2, 1}, {-2, -1}, {1, 2}, {1, -2}, {-1, 2}, {-1, -2}, {2, 2}, {2, -2}, {-2, 2}, {-2, -2}, {-3, 0}, {3, 0}, {0, -3}, {0, 3}}},

};

const static std::vector<ActionTarget> DIRECTION_TARGET_MAP {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};

struct GameObj {
    GameObjType type;
    int hitPoint;
    int resource;
    int owner;
    ActionType currentAction;
    ActionTarget actionTarget;
    int actionProgress;
    int actionTotalProgress;
    int attackCD;
};

struct GameState {
    std::unordered_map<Coord, GameObj, UHasher<Coord>> objMap;
    int w = 0;
    int h = 0;
    int time = 0;

    inline bool operator== (const GameState& state2) const {
        return state2.time == time && state2.w == w && state2.h == h;
    }

    inline bool operator== (const volatile GameState& state2) const {
        return state2.time == time && state2.w == w && state2.h == h;
    }

    inline GameState& operator= (const GameState& state2)  noexcept {
        w = state2.w;
        h = state2.h;
        time = state2.time;
        return *this;
    }
    inline GameState& operator= (const volatile GameState& state2)  noexcept {
        w = state2.w;
        h = state2.h;
        time = state2.time;
        return *this;
    }
};

#endif //RTS_GAME_TYPES_H
