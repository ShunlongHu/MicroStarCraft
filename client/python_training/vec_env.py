from game_types import *
import numpy as np
import torch

WORKER_NUM = 2
def Reset(seed:int,
          isRotSym:bool,
          isAxSym:bool,
          terrainProb:float,
          expansionCnt:int,
          clusterPerExpansion:int,
          mineralPerCluster:int)->(torch.tensor, torch.tensor):
    totalObs = obj.Reset(seed, isRotSym, isAxSym, terrainProb, expansionCnt, clusterPerExpansion, mineralPerCluster)
    ob1 = torch.zeros(WORKER_NUM * OBSERVATION_PLANE_NUM * GAME_H * GAME_W)
    ob2 = torch.zeros(WORKER_NUM * OBSERVATION_PLANE_NUM * GAME_H * GAME_W)
    for i in range(totalObs.ob1.size):
        ob1[i] = totalObs.ob1.data[i]
    for i in range(totalObs.ob2.size):
        ob2[i] = totalObs.ob2.data[i]
    ob1 = ob1.resize(WORKER_NUM, OBSERVATION_PLANE_NUM, GAME_H, GAME_W)
    ob2 = ob2.resize(WORKER_NUM, OBSERVATION_PLANE_NUM, GAME_H, GAME_W)
    return ob1, ob2


if __name__ == "__main__":
    initParam = InitParam(c_int(GAME_W), c_int(GAME_H), c_int(WORKER_NUM))
    print(initParam)
    obj.Init(initParam)
    ob = Reset(0, False, True, 1, 1, 1,100)
    print(ob)
