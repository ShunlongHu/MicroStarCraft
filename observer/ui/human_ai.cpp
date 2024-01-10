//
// Created by zoe50 on 2024/1/10.
//

#include "human_ai.h"
#include "rts_observer.h"
using namespace std;
static int lastIdx = -1;
void HumanAi::Act(const GameState& game, Coord mouseClick, Coord mouseRightClick)
{
    if (RtsObserver::role == OBSERVER) {
        return;
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
    RtsObserver::selectedObj = selType;
}