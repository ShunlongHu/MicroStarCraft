import random

from game import *


def Act(state: GameState) -> dict[int, DiscreteAction]:
    retVal = {}
    for idx, obj in state.objMap.items():
        act = DiscreteAction()
        act.action = random.randint(0, GAME_ACTION_TYPE_NUM - 1)
        act.produceType = random.randint(0, GAME_ACTION_TYPE_NUM - 1)
        act.target.y = random.randint(-state.w, state.w)
        act.target.x = random.randint(-state.h, state.h)
        retVal[idx] = act
    return retVal
