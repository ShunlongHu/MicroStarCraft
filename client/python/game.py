import struct

GAME_OBJ_TYPE_NUM = 8
GAME_PRODUCIBLE_OBJ_TYPE_NUM = 6
GAME_ACTION_TYPE_NUM = 6
GAME_DIRECTION_TYPE_NUM = 4

TERRAIN = 0
MINERAL = 1
BASE = 2
BARRACK = 3
WORKER = 4
LIGHT = 5
HEAVY = 6
RANGED = 7

OBJ_HP_MAP = {
    BASE: 60,
    BARRACK: 40,
    WORKER: 4,
    LIGHT: 4,
    HEAVY: 12,
    RANGED: 4
}

OBJ_ATTACK_MAP = {
    WORKER: 1,
    LIGHT: 1,
    HEAVY: 4,
    RANGED: 1
}

OBJ_ATTACK_INTERVAL_MAP = {
    WORKER: 2,
    LIGHT: 1,
    HEAVY: 2,
    RANGED: 1
}

OBJ_COST_MAP = {
    BASE: 16,
    BARRACK: 6,
    WORKER: 2,
    LIGHT: 1,
    HEAVY: 4,
    RANGED: 2
}

OBJ_TIME_MAP = {
    BASE: 25,
    BARRACK: 16,
    WORKER: 4,
    LIGHT: 3,
    HEAVY: 7,
    RANGED: 6
}

OBJ_MOVE_INTERVAL_MAP = {
    WORKER: 1,
    LIGHT: 0,
    HEAVY: 1,
    RANGED: 1
}

OBJ_PRODUCE_MAP = {
    BASE: {WORKER},
    BARRACK: {LIGHT, HEAVY, RANGED},
    WORKER: {BASE, BARRACK}
}

OBJ_ACTION_MASK_MAP = {
    TERRAIN: (False, False, False, False, False, False),
    MINERAL: (False, False, False, False, False, True),
    BASE: (False, False, False, True, True, False),
    BARRACK: (False, False, False, False, True, False),
    WORKER: (True, True, True, False, True, False),
    LIGHT: (True, True, False, False, True, False),
    HEAVY: (True, True, False, False, True, False),
    RANGED: (True, True, False, False, True, False)
}

OBJ_BUILDING_SET = {BASE, BARRACK}

OBJ_ATTACK_RANGE_MAP = {
    TERRAIN: 0,
    MINERAL: 0,
    BASE: 0,
    BARRACK: 0,
    WORKER: 1,
    LIGHT: 1,
    HEAVY: 1,
    RANGED: 3,
}

RES_PER_GATHER = 1
GATHER_TIME = 1

NOOP = 0
MOVE = 1
GATHER = 2
RETURN = 3
PRODUCE = 4
ATTACK = 5


class Coord:
    def __init__(self, y=-1, x=-1):
        self.y = y
        self.x = x


class ActionMask:
    def __init__(self):
        self.canMove = False
        self.canAttack = False
        self.canGather = False
        self.canBeStored = False
        self.canBeAttacked = False
        self.canBeGathered = False


class GameObj:
    def __init__(self):
        self.objType = TERRAIN
        self.coord = Coord(-1, -1)
        self.hitPoint = -1
        self.resource = -1
        self.owner = 0
        self.currentAction = NOOP
        self.actionTarget = Coord(-1, -1)
        self.actionProgress = -1
        self.actionTotalProgress = -1
        self.attackCD = -1
        self.produceType = TERRAIN
        self.attackRange = -1
        self.attackInterval = -1
        self.attackPoint = -1
        self.actionMask = ActionMask()


class GameState:
    def __init__(self):
        self.objMap = dict()
        self.resource = [0, 0]
        self.buildingCnt = [0, 0]
        self.w = 1
        self.h = 1
        self.time = 0
        self.objCnt = 0


def ByteStreamToGameState(byteStream: bytes) -> GameState:
    state = GameState()
    idx = 0
    objNum = int.from_bytes(byteStream[idx: idx + 8], "little", signed=True)
    idx += 8
    for i in range(objNum):
        objIdx = int.from_bytes(byteStream[idx: idx + 4], "little", signed=True)
        idx += 4
        obj = GameObj()
        obj.type = int.from_bytes(byteStream[idx: idx + 4], "little", signed=True)
        idx += 4
        obj.coord.y = int.from_bytes(byteStream[idx: idx + 4], "little", signed=True)
        idx += 4
        obj.coord.x = int.from_bytes(byteStream[idx: idx + 4], "little", signed=True)
        idx += 4
        obj.hitPoint = int.from_bytes(byteStream[idx: idx + 2], "little", signed=True)
        idx += 2
        obj.resource = int.from_bytes(byteStream[idx: idx + 2], "little", signed=True)
        idx += 2
        obj.owner = int.from_bytes(byteStream[idx: idx + 1], "little", signed=True)
        idx += 1
        obj.currentAction = int.from_bytes(byteStream[idx: idx + 4], "little", signed=True)
        idx += 4
        obj.actionTarget.y = int.from_bytes(byteStream[idx: idx + 4], "little", signed=True)
        idx += 4
        obj.actionTarget.x = int.from_bytes(byteStream[idx: idx + 4], "little", signed=True)
        idx += 4
        obj.actionProgress = int.from_bytes(byteStream[idx: idx + 2], "little", signed=True)
        idx += 2
        obj.actionTotalProgress = int.from_bytes(byteStream[idx: idx + 2], "little", signed=True)
        idx += 2
        obj.attackCD = int.from_bytes(byteStream[idx: idx + 2], "little", signed=True)
        idx += 2
        obj.produceType = int.from_bytes(byteStream[idx: idx + 4], "little", signed=True)
        idx += 4
        obj.attackRange = int.from_bytes(byteStream[idx: idx + 2], "little", signed=True)
        idx += 2
        obj.attackInterval = int.from_bytes(byteStream[idx: idx + 2], "little", signed=True)
        idx += 2
        obj.attackPoint = int.from_bytes(byteStream[idx: idx + 2], "little", signed=True)
        idx += 2
        obj.actionMask.canMove = int.from_bytes(byteStream[idx: idx + 1], "little", signed=True)
        idx += 1
        obj.actionMask.canAttack = int.from_bytes(byteStream[idx: idx + 1], "little", signed=True)
        idx += 1
        obj.actionMask.canGather = int.from_bytes(byteStream[idx: idx + 1], "little", signed=True)
        idx += 1
        obj.actionMask.canBeStored = int.from_bytes(byteStream[idx: idx + 1], "little", signed=True)
        idx += 1
        obj.actionMask.canBeAttacked = int.from_bytes(byteStream[idx: idx + 1], "little", signed=True)
        idx += 1
        obj.actionMask.canBeGathered = int.from_bytes(byteStream[idx: idx + 1], "little", signed=True)
        idx += 1
        state.objMap[objIdx] = obj

    state.resource[0] = int.from_bytes(byteStream[idx: idx + 4], "little", signed=True)
    idx += 4
    state.resource[1] = int.from_bytes(byteStream[idx: idx + 4], "little", signed=True)
    idx += 4
    state.buildingCnt[0] = int.from_bytes(byteStream[idx: idx + 4], "little", signed=True)
    idx += 4
    state.buildingCnt[1] = int.from_bytes(byteStream[idx: idx + 4], "little", signed=True)
    idx += 4
    state.w = int.from_bytes(byteStream[idx: idx + 4], "little", signed=True)
    idx += 4
    state.h = int.from_bytes(byteStream[idx: idx + 4], "little", signed=True)
    idx += 4
    state.time = int.from_bytes(byteStream[idx: idx + 4], "little", signed=True)
    idx += 4
    state.objCnt = int.from_bytes(byteStream[idx: idx + 4], "little", signed=True)
    idx += 4
    return state


class DiscreteAction:
    def __init__(self):
        self.action = NOOP
        self.produceType = TERRAIN
        self.target = Coord(0, 0)


class TotalDiscreteAction:
    def __init__(self):
        self.action = [{}, {}]


def ActionToByteStream(actionDict: dict[int, DiscreteAction]) -> bytes:
    byteStream = b''
    byteStream += int.to_bytes(len(actionDict), 4, "little", signed=True)
    for k, v in actionDict.items():
        byteStream += int.to_bytes(k, 4, "little", signed=True)
        byteStream += int.to_bytes(v.produceType, 4, "little", signed=True)
        byteStream += int.to_bytes(v.target.y, 4, "little", signed=True)
        byteStream += int.to_bytes(v.target.x, 4, "little", signed=True)
