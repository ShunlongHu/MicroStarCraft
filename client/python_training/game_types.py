from ctypes import *


class InitParam(Structure):
    _fields_ = [("w", c_int), ("h", c_int), ("numWorkers", c_int)]


class Observation(Structure):
    _fields_ = [("data", POINTER(c_byte)),
                ("size", c_int),
                ("reward", POINTER(c_int)),
                ]


class Action(Structure):
    _fields_ = [("data", POINTER(c_byte)), ("size", c_int)]


class TotalObservation(Structure):
    _fields_ = [("ob1", Observation), ("ob2", Observation), ("time", c_int), ("isEnd", c_bool), ("winningSide", c_int)]


class TotalAction(Structure):
    _fields_ = [("action1", Action), ("action2", Action)]


class Reward:
    NEW_WORKER_CNT = 0
    NEW_LIGHT_CNT = 1
    NEW_RANGED_CNT = 2
    NEW_HEAVY_CNT = 3
    NEW_BASE_CNT = 4
    NEW_BARRACK_CNT = 5

    DEAD_WORKER_CNT = 6
    DEAD_LIGHT_CNT = 7
    DEAD_RANGED_CNT = 8
    DEAD_HEAVY_CNT = 9
    DEAD_BASE_CNT = 10
    DEAD_BARRACK_CNT = 11

    NEW_WORKER_KILLED = 12
    NEW_LIGHT_KILLED = 13
    NEW_RANGED_KILLED = 14
    NEW_HEAVY_KILLED = 15
    NEW_BASE_KILLED = 16
    NEW_BARRACK_KILLED = 17

    NEW_NET_INCOME = 18
    NEW_HIT_CNT = 19
