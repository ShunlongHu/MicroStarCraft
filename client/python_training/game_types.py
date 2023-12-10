from ctypes import *


class InitParam(Structure):
    _fields_ = [("w", c_int), ("h", c_int), ("numWorkers", c_int)]


class Observation(Structure):
    _fields_ = [("data", POINTER(c_byte)),
                ("size", c_int),

                ("newWorkerCnt", c_int),
                ("newLightCnt", c_int),
                ("newRangedCnt", c_int),
                ("newHeavyCnt", c_int),
                ("newBaseCnt", c_int),
                ("newBarrackCnt", c_int),

                ("deadWorkerCnt", c_int),
                ("deadLightCnt", c_int),
                ("deadRangedCnt", c_int),
                ("deadHeavyCnt", c_int),
                ("deadBaseCnt", c_int),
                ("deadBarrackCnt", c_int),

                ("newWorkerKilled", c_int),
                ("newLightKilled", c_int),
                ("newRangedKilled", c_int),
                ("newHeavyKilled", c_int),
                ("newBaseKilled", c_int),
                ("newBarrackKilled", c_int),

                ("newNetIncome", c_int),
                ("newHitCnt", c_int)
                ]


class Action(Structure):
    _fields_ = [("data", POINTER(c_byte)), ("size", c_int)]


class TotalObservation(Structure):
    _fields_ = [("ob1", Observation), ("ob2", Observation), ("time", c_int), ("isEnd", c_bool), ("winningSide", c_int)]


class TotalAction(Structure):
    _fields_ = [("action1", Action), ("action2", Action)]
