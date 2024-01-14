//
// Created by zoe50 on 2024/1/10.
//

#include "human_ai.h"
#include "rts_observer.h"
#include "rpc_client.h"
using namespace std;
static int lastIdx = -1;
static unordered_map<int, DiscreteAction> actionMap;

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
    auto action = RtsObserver::gameAction == -1;
    if (curIdx != -1 && mouseRightClick.y != -1) {
        if (coordIdxMap.count(mouseRightClick)) {
            auto idx = coordIdxMap.at(mouseRightClick);
            GameObjType type = TERRAIN;
            if (game.objMap.count(idx)) {
                type = game.objMap.at(idx).type;
            }
            auto player = RtsObserver::role == PLAYER_A ? -1 : 1;
            actionMap.emplace(curIdx, DiscreteAction{type == MINERAL ? GATHER : type == BASE && game.objMap.at(idx).owner == player ? RETURN : ATTACK, TERRAIN, ActionTarget{mouseRightClick.y, mouseRightClick.x}});
        } else if (action) {
            actionMap.emplace(curIdx, DiscreteAction{MOVE, TERRAIN, ActionTarget{mouseRightClick.y, mouseRightClick.x}});
        } else {
            auto& obj = game.objMap.at(curIdx);
            actionMap.emplace(curIdx, DiscreteAction{PRODUCE, OBJ_PRODUCE_MAP.at(obj.type)[RtsObserver::gameAction], ActionTarget{mouseRightClick.y, mouseRightClick.x}});
        }
    }

    RpcClient::SetAction(actionMap);
    if (!actionMap.empty()) {
        RpcClient::SendCommand(message::STEP);
    }
    actionMap = {};
    RtsObserver::selectedObj = selType;
}