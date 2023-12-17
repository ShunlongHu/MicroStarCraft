//
// Created by zoe50 on 2023/12/10.
//

#ifndef RTS_GAME_TYPES_H
#define RTS_GAME_TYPES_H

#include <unordered_map>
#include <vector>
#include <iostream>

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

const static std::unordered_map<GameObjType, int> OBJ_HP_MAP{  // sc2 / 25
        {BASE,    60},
        {BARRACK, 40},
        {WORKER,  4},
        {LIGHT,   4},
        {HEAVY,   12},
        {RANGED,  4}
};

const static std::unordered_map<GameObjType, int> OBJ_ATTACK_MAP{ // sc2 / 40
        {WORKER, 1},
        {LIGHT,  1},
        {HEAVY,  4},
        {RANGED, 1}
};

const static std::unordered_map<GameObjType, int> OBJ_ATTACK_INTERVAL_MAP{ // sc2 * 2
        {WORKER, 1},
        {LIGHT,  0},
        {HEAVY,  1},
        {RANGED, 0}
};

const static std::unordered_map<GameObjType, int> OBJ_COST_MAP{ // sc2 / 50
        {BASE,    16},
        {BARRACK, 6},
        {WORKER,  2},
        {LIGHT,   1},
        {HEAVY,   4},
        {RANGED,  2}
};

const static std::unordered_map<GameObjType, int> OBJ_TIME_MAP{ // sc2 / 4
        {BASE,    25},
        {BARRACK, 16},
        {WORKER,  4},
        {LIGHT,   3},
        {HEAVY,   7},
        {RANGED,  6}
};

const static std::unordered_map<GameObjType, int> OBJ_MOVE_INTERVAL_MAP{ // (移动前摇) sc2 * 2
        {WORKER, 1},
        {LIGHT,  0},
        {HEAVY,  1},
        {RANGED, 1}
};

const static std::unordered_map<GameObjType, std::vector<GameObjType>> OBJ_PRODUCE_MAP{ // sc2 / 50
        {BASE,    {WORKER}},
        {BARRACK, {LIGHT, HEAVY, RANGED}},
        {WORKER,  {BASE,  BARRACK}},
};

struct ActionMask {
    bool canMove;
    bool canAttack;
    bool canGather;
    bool canBeStored; // can be stored resource to
    bool canBeAttacked;
    bool canBeGathered;
};

const static std::unordered_map<GameObjType, ActionMask> OBJ_ACTION_MASK_MAP{ // sc2 / 50
        {TERRAIN, {false, false, false, false, false, false}},
        {MINERAL, {false, false, false, false, false, true}},
        {BASE,    {false, false, false, true,  true,  false}},
        {BARRACK, {false, false, false, false, true,  false}},
        {WORKER,  {true,  true,  true,  false, true,  false}},
        {LIGHT,   {true,  true,  false, false, true,  false}},
        {HEAVY,   {true,  true,  false, false, true,  false}},
        {RANGED,  {true,  true,  false, false, true,  false}},
};

const static std::unordered_set<GameObjType> OBJ_BUILDING_SET{
        {BASE, BARRACK}
};

// GAME_ECONOMY
constexpr static int RES_PER_GATHER = 1;
constexpr static int GATHER_POST = 0;

template<class T>
struct UHasher {
    size_t operator()(const T &key) const {
        if (sizeof(T) == 8) {
            return *reinterpret_cast<const uint64_t *>(&key);
        }
        if (sizeof(T) == 4) {
            return *reinterpret_cast<const uint32_t *>(&key);
        }
        if (sizeof(T) == 2) {
            return *reinterpret_cast<const uint16_t *>(&key);
        }
        if (sizeof(T) == 1) {
            return *reinterpret_cast<const uint8_t *>(&key);
        }
        return std::_Hash_array_representation(reinterpret_cast<const char *>(&key), sizeof(key));
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

    bool operator==(const Coord &second) const {
        return second.x == x && second.y == y;
    }

    inline Coord &operator=(const Coord &state2) noexcept {
        this->x = state2.x;
        this->y = state2.y;
        return *this;
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
        {1, {{-1, 0}, {1, 0},  {0,  -1}, {0,  1}}},
        /*   xxx
         *  x o x
         *   xxx
         */
        {2, {{1,  1}, {1, -1}, {-1, 1},  {-1, -1}, {-2, 0}, {2, 0},  {0,  -2}, {0,  2}}},
        /*
         *    x
         *  xx xx
         *  x   x
         * x  o  x
         *  x   x
         *  xx xx
         *    x
         */
        {3, {{2,  1}, {2, -1}, {-2, 1},  {-2, -1}, {1,  2}, {1, -2}, {-1, 2},  {-1, -2}, {2, 2}, {2, -2}, {-2, 2}, {-2, -2}, {-3, 0}, {3, 0}, {0, -3}, {0, 3}}},

};

const static std::vector<ActionTarget> DIRECTION_TARGET_MAP{{-1, 0},
                                                            {0,  1},
                                                            {1,  0},
                                                            {0,  -1}};

struct GameObj {
    GameObjType type;
    uint8_t hitPoint;
    uint8_t resource;   // resource of a cluster/carried by a worker
    int8_t owner;   // -1: a, 0: neutral, 1:b
    ActionType currentAction;
    ActionTarget actionTarget;
    uint8_t actionProgress;
    uint8_t actionTotalProgress;
    uint8_t attackCD;

    // static attributes
    uint8_t attackRange;
    uint8_t attackInterval;
    uint8_t attackPoint;
    uint8_t moveInterval;

    // action mask
    bool canMove;
    bool canAttack;
    bool canGather;
    bool canStore; // can store resource
    bool canBeAttacked;
    bool canBeGathered;

    inline bool operator==(const GameObj &state2) const {
        auto ptrA = reinterpret_cast<const char *> (this);
        auto ptrB = reinterpret_cast<const char *> (&state2);
        for (int i = 0; i < sizeof(GameObj); ++i) {
            if (ptrA[i] != ptrB[i]) {
                return false;
            }
        }
        return true;
    }

    inline GameObj &operator=(const GameObj &state2) noexcept {
        auto ptrA = reinterpret_cast<char *> (this);
        auto ptrB = reinterpret_cast<const char *> (&state2);
        for (int i = 0; i < sizeof(GameObj); ++i) {
            ptrA[i] = ptrB[i];
        }
        return *this;
    }
};

struct GameState {
    std::unordered_map<Coord, GameObj, UHasher<Coord>> objMap;
    int resource[2]{0, 0};
    int buildingCnt[2]{0, 0};
    int w = 0;
    int h = 0;
    int time = 0;

    inline bool operator==(const GameState &state2) const {
        return state2.time == time && state2.w == w && state2.h == h && resource[0] == state2.resource[0] &&
               resource[1] == state2.resource[1] && objMap == state2.objMap;
    }

    inline GameState &operator=(const GameState &state2) noexcept {
        w = state2.w;
        h = state2.h;
        resource[0] = state2.resource[0];
        resource[1] = state2.resource[1];
        time = state2.time;
        objMap = state2.objMap;
        return *this;
    }
};

template<class T>
void Serialize(std::ostream &os, const T &t) {
    os.write(reinterpret_cast<const char *>(&t), sizeof(t));
}

template<class T>
void Deserialize(std::istream &is, T &t) {
    is.read(reinterpret_cast<char *>(&t), sizeof(t));
}

template<class K, class V, class H>
std::ostream &operator<<(std::ostream &os, const std::unordered_map<K, V, H> &ht) {
    auto size = ht.size();
    Serialize(os, size);
    for (const auto &[k, v]: ht) {
        Serialize(os, k);
        Serialize(os, v);
    }
    return os;
}

template<class K, class V, class H>
std::istream &operator>>(std::istream &is, std::unordered_map<K, V, H> &ht) {
    size_t size;
    Deserialize(is, size);
    ht.reserve(size);
    for (size_t i = 0; i < size; ++i) {
        K key;
        V value;
        Deserialize(is, key);
        Deserialize(is, value);
        ht.emplace(key, value);
    }
    return is;
}

inline std::ostream &operator<<(std::ostream &os, const GameState &state) {
    os << state.objMap;
    Serialize(os, state.resource[0]);
    Serialize(os, state.resource[1]);
    Serialize(os, state.buildingCnt[0]);
    Serialize(os, state.buildingCnt[1]);
    Serialize(os, state.w);
    Serialize(os, state.h);
    Serialize(os, state.time);
    return os;
}

inline std::istream &operator>>(std::istream &is, GameState &state) {
    is >> state.objMap;
    Deserialize(is, state.resource[0]);
    Deserialize(is, state.resource[1]);
    Serialize(is, state.buildingCnt[0]);
    Serialize(is, state.buildingCnt[1]);
    Deserialize(is, state.w);
    Deserialize(is, state.h);
    Deserialize(is, state.time);
    return is;
}

#endif //RTS_GAME_TYPES_H
