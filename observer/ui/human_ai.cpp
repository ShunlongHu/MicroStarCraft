//
// Created by zoe50 on 2024/1/10.
//

#include "human_ai.h"
#include <queue>
#include "rts_observer.h"
#include "rpc_client.h"
using namespace std;
static int lastIdx = -1;
static unordered_map<int, DiscreteAction> actionMap;
static unordered_map<int, DiscreteAction> txActionMap;
static vector<bool> occupationMap;
static GameState lastGame;

void ProcAction(const GameState& game, const unordered_map<Coord, int, UHasher<Coord>>& coordIdxMap);
void ProcMove(const GameState& game, int idx, const ActionTarget & target);

void HumanAi::Act(const GameState& game, Coord mouseClick, Coord mouseRightClick)
{
    if (RtsObserver::role == OBSERVER) {
        return;
    }
    if (!(lastGame == game)) {
        txActionMap = {};
        lastGame = game;
    }
    unordered_map<Coord, int, UHasher<Coord>> coordIdxMap;
    for (const auto& [idx, obj]: game.objMap) {
        coordIdxMap.emplace(obj.coord, idx);
    }
    GameObjType selType {TERRAIN};
    int curIdx = -1;
    auto iter = coordIdxMap.find(mouseClick);
    if (iter != coordIdxMap.end()) {
        auto& [coord, idx] = *iter;
        auto& obj = game.objMap.at(idx);
        if ((obj.owner == -1 && RtsObserver::role == PLAYER_A) ||
            (obj.owner == 1 && RtsObserver::role == PLAYER_B)) {
            selType = obj.type;
            curIdx = idx;
        }
    }
    if (lastIdx != curIdx) {
        RtsObserver::gameAction = -1;
    }
    lastIdx = curIdx;
    auto action = RtsObserver::gameAction == -1;
    if (curIdx != -1 && mouseRightClick.y != -1) {
        if (coordIdxMap.count(mouseRightClick)) {
            auto idx = coordIdxMap.at(mouseRightClick);
            GameObjType type = TERRAIN;
            if (game.objMap.count(idx)) {
                type = game.objMap.at(idx).type;
            }
            auto player = RtsObserver::role == PLAYER_A ? -1 : 1;
            actionMap[curIdx] = DiscreteAction{type == MINERAL ? GATHER : type == BASE && game.objMap.at(idx).owner == player ? RETURN : ATTACK, TERRAIN, ActionTarget{mouseRightClick.y, mouseRightClick.x}};
        } else if (action) {
            actionMap[curIdx] = DiscreteAction{MOVE, TERRAIN, ActionTarget{mouseRightClick.y, mouseRightClick.x}};
        } else {
            auto& obj = game.objMap.at(curIdx);
            actionMap[curIdx] = DiscreteAction{PRODUCE, OBJ_PRODUCE_MAP.at(obj.type)[RtsObserver::gameAction], ActionTarget{mouseRightClick.y, mouseRightClick.x}};
        }
    }
    ProcAction(game, coordIdxMap);

    RpcClient::SetAction(txActionMap);
    if (!txActionMap.empty()) {
        RpcClient::SendCommand(message::STEP);
    }
    RtsObserver::selectedObj = selType;
}

void ProcAction(const GameState& game, const unordered_map<Coord, int, UHasher<Coord>>& coordIdxMap) {
    occupationMap = vector<bool>(game.w * game.h, false);

    unordered_map<int, GameObj> baseMap;
    for (const auto& [i, obj]: game.objMap) {
        occupationMap[obj.coord.x + obj.coord.y * game.w] = true;
        if (obj.currentAction == MOVE || obj.currentAction == PRODUCE) {
            occupationMap[obj.actionTarget.x + obj.actionTarget.y * game.w] = true;
        }

        if ((obj.owner == -1 && RtsObserver::role == PLAYER_A) ||
         (obj.owner == 1 && RtsObserver::role == PLAYER_B)) {
            if (obj.type == BASE) {
                baseMap.emplace(i, obj);
            }
        }
    }


    unordered_set<int> doneActionSet;
    unordered_set<Coord, UHasher<Coord>> toGatherSet;
    for (const auto& [idx, act]: actionMap) {
        if (game.objMap.count(idx) == 0) {
            doneActionSet.emplace(idx);
            continue;
        }
        const auto& obj = game.objMap.at(idx);
        if (obj.currentAction != NOOP) {
            continue;
        }
        int nearestBase = -1;
        double nearestDistance = game.w + game.h;
        Coord baseCoord;
        auto opponent = RtsObserver::role == PLAYER_A ? 1 : -1;
        switch (act.action) {
            case PRODUCE:
                txActionMap[idx] = act;
                doneActionSet.emplace(idx);
                break;
            case ATTACK:
                if (coordIdxMap.count(act.target) == 0 || game.objMap.at(coordIdxMap.at(act.target)).owner != opponent) {
                    doneActionSet.emplace(idx);
                    break;
                }
                if (obj.attackRange >= sqrt(
                        (act.target.y - obj.coord.y) * (act.target.y - obj.coord.y) +
                        (act.target.x - obj.coord.x) * (act.target.x - obj.coord.x))) {
                    // within range: attack
                    txActionMap[idx] = act;
                    break;
                }
                ProcMove(game, idx, act.target);
                break;
            case MOVE:
                if (obj.coord.x == act.target.x && obj.coord.y == act.target.y) {
                    doneActionSet.emplace(idx);
                    break;
                }
                ProcMove(game, idx, act.target);
                break;
            case RETURN:
                if (abs(obj.coord.x - act.target.x) + abs(obj.coord.y - act.target.y) <= 1) {
                    txActionMap[idx] = act;
                    doneActionSet.emplace(idx);
                    break;
                }
                ProcMove(game, idx, act.target);
                break;
            case GATHER:
                if (obj.resource == 0) {
                    if (coordIdxMap.count(act.target) == 0) {
                        // mineral depleted
                        doneActionSet.emplace(idx);
                        break;
                    }
                    // Go Gather
                    if (abs(obj.coord.x - act.target.x) + abs(obj.coord.y - act.target.y) <= 1) {
                        if (toGatherSet.count(act.target)) {
                            break;
                        }
                        txActionMap[idx] = act;
                        toGatherSet.emplace(act.target);
                        break;
                    }
                    // go to mineral
                    ProcMove(game, idx, act.target);
                    break;
                }
                // Return Resource
                for (const auto& [baseIdx, base]: baseMap) {
                    double distance = sqrt((base.coord.y - obj.coord.y) * (base.coord.y - obj.coord.y) +
                    (base.coord.x - obj.coord.x) * (base.coord.x - obj.coord.x));
                    if (distance < nearestDistance) {
                        nearestBase = baseIdx;
                        nearestDistance = distance;
                        baseCoord = base.coord;
                    }
                }
                if (nearestBase == -1) {
                    // no base
                    break;
                }

                if (abs(obj.coord.x - baseCoord.x) + abs(obj.coord.y - baseCoord.y) <= 1) {
                    // reach base
                    txActionMap[idx] = DiscreteAction{RETURN, TERRAIN, baseCoord};
                    break;
                }
                // goto base
                ProcMove(game, idx, baseCoord);
                break;
            default:
                doneActionSet.emplace(idx);
                break;
        }
    }
    // remove done actions
    for (const auto& idx: doneActionSet) {
        actionMap.erase(idx);
    }

    // auto attack
    unordered_map<int, DiscreteAction> autoAttackAction;
    for (const auto& [idx, obj]: game.objMap) {
        if (txActionMap.count(idx) || obj.currentAction != NOOP) {
            continue;
        }
        if ((obj.owner == -1 && RtsObserver::role == PLAYER_B) ||
            (obj.owner == 1 && RtsObserver::role == PLAYER_A)) {
            // not own unit
            continue;
        }
        if (!obj.actionMask.canAttack) {
            continue;
        }
        unordered_set<int> targetObjSet;
        for (const auto& [ti, target]: game.objMap) {
            if (target.owner == obj.owner) {
                continue;
            }
            if (!target.actionMask.canBeAttacked) {
                continue;
            }
            if (sqrt((target.coord.y - obj.coord.y) * (target.coord.y - obj.coord.y) +
                     (target.coord.x - obj.coord.x) * (target.coord.x - obj.coord.x)) > obj.attackRange) {
                continue;
            }
            targetObjSet.emplace(ti);
        }
        if (targetObjSet.empty()) {
            continue;
        }
        int closestIdx = -1;
        Coord closestCoord;
        double closestDistance = game.w + game.h;
        for (const auto& tgtIdx: targetObjSet) {
            const auto& tgt = game.objMap.at(tgtIdx);
            double distance = sqrt((tgt.coord.y - obj.coord.y) * (tgt.coord.y - obj.coord.y) +
                                   (tgt.coord.x - obj.coord.x) * (tgt.coord.x - obj.coord.x));
            if (distance < closestDistance) {
                closestIdx = tgtIdx;
                closestDistance = distance;
                closestCoord = tgt.coord;
            }
        }
        if (closestIdx != -1) {
            txActionMap[idx] = DiscreteAction{ATTACK, TERRAIN, closestCoord};
        }
    }
}

struct Loc {
    Coord coord;
    int parentIdx;
    int d;
    int h;

    bool operator < (const Loc& key2) const {
        return d + h < (key2.d + key2.h);
    }
    bool operator > (const Loc& key2) const {
        return d + h > (key2.d + key2.h);
    }

    inline Loc &operator=(const Loc &state2) noexcept {
        this->coord = state2.coord;
        this->d = state2.d;
        this->h = state2.h;
        this->parentIdx = state2.parentIdx;
        return *this;
    }
    Loc() : d(0), h(0), parentIdx(-1) {}
    Loc(const Coord& coord, int parent, int d, int h) : coord(coord), parentIdx(parent), d(d), h(h) {}
};

inline int heuristic(const Coord& a, const Coord& b) {
    return abs(a.x - b.x) + abs(a.y - b.y);
}

void ProcMove(const GameState& game, int idx, const ActionTarget & target) {
    const auto& coord = game.objMap.at(idx).coord;
    unordered_set<Coord, UHasher<Coord>> visited;
    vector<Loc> path;
    priority_queue<Loc, vector<Loc>, std::greater<>> pq;
    pq.emplace(coord, -1, 0, heuristic(coord, target));
    while (!pq.empty()) {
        auto front = pq.top();
        if (front.coord.x == target.x && front.coord.y == target.y) {
            break;
        }
        pq.pop();
        path.emplace_back(front);
        visited.emplace(front.coord);
        for (const auto& dir: DIRECTION_TARGET_MAP) {
            auto next = Coord{dir.y + front.coord.y, dir.x + front.coord.x};
            if (next.x < 0 || next.y < 0 || next.x >= game.w || next.y >= game.h) {
                continue;
            }
            if (occupationMap[next.x + next.y * game.w] && !(next.x == target.x && next.y == target.y)) {
                continue;
            }
            if (visited.count(next)) {
                continue;
            }
            pq.emplace(next, static_cast<int>(path.size()) - 1, 1 + front.d, heuristic(next, target));
        }
    }
    if (pq.empty()){
        return;
    }
    auto final = pq.top();
    if (!(final.coord == target)) {
        return;
    }
    auto last = final;
    while (last.parentIdx != 0 && last.parentIdx != -1 ) {
        last = path[last.parentIdx];
    }
    txActionMap[idx] =  DiscreteAction{MOVE, TERRAIN, last.coord};
    occupationMap[last.coord.x + last.coord.y * game.w] = true;
}