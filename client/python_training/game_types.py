from ctypes import *


class InitParam(Structure):
    _fields_ = [("w", c_int), ("h", c_int), ("numWorkers", c_int)]


class Observation(Structure):
    _fields_ = [("data", POINTER(c_byte)),
                ("size", c_int),
                ("reward", POINTER(c_int)),
                ("rewardSize", c_int),
                ]


class Action(Structure):
    _fields_ = [("data", POINTER(c_byte)), ("size", c_int)]


class TotalObservation(Structure):
    _fields_ = [("ob1", Observation), ("ob2", Observation)]


class TotalAction(Structure):
    _fields_ = [("action1", Action), ("action2", Action)]


class Reward:
    GAME_TIME = 0
    IS_END = 1
    VICTORY_SIDE = 2
    NEW_WORKER_CNT = 3
    NEW_LIGHT_CNT = 4
    NEW_RANGED_CNT = 5
    NEW_HEAVY_CNT = 6
    NEW_BASE_CNT = 7
    NEW_BARRACK_CNT = 8

    DEAD_WORKER_CNT = 9
    DEAD_LIGHT_CNT = 10
    DEAD_RANGED_CNT = 11
    DEAD_HEAVY_CNT = 12
    DEAD_BASE_CNT = 13
    DEAD_BARRACK_CNT = 14

    NEW_WORKER_KILLED = 15
    NEW_LIGHT_KILLED = 16
    NEW_RANGED_KILLED = 17
    NEW_HEAVY_KILLED = 18
    NEW_BASE_KILLED = 19
    NEW_BARRACK_KILLED = 20
    NEW_NET_INCOME = 21
    NEW_HIT_CNT = 22


# action, move dir, gather dir, return dir, prod dir, prod type, attack dir
ACTION_SIZE = [6, 4, 4, 4, 4, 6, 49]

GAME_W = 32
GAME_H = 32
OBSERVATION_PLANE_NUM = 30

obj = cdll.LoadLibrary("D:/repo/rts/cmake-build-release/server/engine/rts_engine_shared.dll")
obj.Reset.argtypes = [c_int, c_bool, c_bool, c_double, c_int, c_int, c_int]
obj.Reset.restype = TotalObservation