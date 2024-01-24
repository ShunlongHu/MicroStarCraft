import time
from game_types import *
import numpy as np
import matplotlib
matplotlib.use('TkAgg',force=True)
from matplotlib import pyplot as plt
print("Switched to:",matplotlib.get_backend())
import torch


class VecEnv:
    def __init__(self, num_workers: int, device: torch.device):
        self.device = device
        self.num_workers = num_workers
        initParam = InitParam(c_int(GAME_W), c_int(GAME_H), c_int(self.num_workers))
        obj.Init(initParam)

    def reset(self, seed: int,
              isRotSym: bool,
              isAxSym: bool,
              terrainProb: float,
              expansionCnt: int,
              clusterPerExpansion: int,
              mineralPerCluster: int) -> (torch.tensor, torch.tensor):
        totalObs = obj.Reset(seed, isRotSym, isAxSym, terrainProb, expansionCnt, clusterPerExpansion, mineralPerCluster)
        assert totalObs.ob1.size == self.num_workers * OBSERVATION_PLANE_NUM * GAME_H * GAME_W
        assert totalObs.ob2.size == self.num_workers * OBSERVATION_PLANE_NUM * GAME_H * GAME_W
        ob1 = torch.from_numpy(np.ctypeslib.as_array(totalObs.ob1.data, [self.num_workers, OBSERVATION_PLANE_NUM, GAME_H, GAME_W]))
        ob2 = torch.from_numpy(np.ctypeslib.as_array(totalObs.ob2.data, [self.num_workers, OBSERVATION_PLANE_NUM, GAME_H, GAME_W]))
        return ob1, ob2

    def step(self, action1: torch.tensor, action2: torch.tensor) -> ((torch.tensor, torch.tensor), (torch.tensor, torch.tensor), torch.tensor, str):
        actionData1 = action1.reshape(self.num_workers * len(ACTION_SIZE) * GAME_H * GAME_W).type(torch.uint8)
        actionData2 = action2.reshape(self.num_workers * len(ACTION_SIZE) * GAME_H * GAME_W).type(torch.uint8)
        actionObj1 = actionData1.numpy().ctypes.data_as(POINTER(c_byte))
        actionObj2 = actionData2.numpy().ctypes.data_as(POINTER(c_byte))
        actionStruct1 = Action(actionObj1, c_int(actionData1.size(0)))
        actionStruct2 = Action(actionObj2, c_int(actionData2.size(0)))

        totalObs = obj.Step(TotalAction(actionStruct1, actionStruct2))
        assert totalObs.ob1.size == self.num_workers * OBSERVATION_PLANE_NUM * GAME_H * GAME_W
        assert totalObs.ob2.size == self.num_workers * OBSERVATION_PLANE_NUM * GAME_H * GAME_W
        ob1 = torch.from_numpy(np.ctypeslib.as_array(totalObs.ob1.data, [self.num_workers, OBSERVATION_PLANE_NUM, GAME_H, GAME_W]))
        ob2 = torch.from_numpy(np.ctypeslib.as_array(totalObs.ob2.data, [self.num_workers, OBSERVATION_PLANE_NUM, GAME_H, GAME_W]))

        ob1 = ob1.to(self.device)
        ob2 = ob2.to(self.device)

        ob1 = ob1 * 2 - 1
        ob2 = ob2 * 2 - 1

        re1 = torch.zeros(self.num_workers)
        re2 = torch.zeros(self.num_workers)
        isEnd = torch.zeros(self.num_workers)
        print("time = ", totalObs.ob1.reward[Reward.GAME_TIME])
        for i in range(self.num_workers):
            isEnd[i] = totalObs.ob1.reward[i * REWARD_SIZE + Reward.IS_END]
            re1[i] += totalObs.ob1.reward[i * REWARD_SIZE + Reward.NEW_NET_INCOME]
            re1[i] += totalObs.ob1.reward[i * REWARD_SIZE + Reward.NEW_WORKER_CNT]
            re1[i] += totalObs.ob1.reward[i * REWARD_SIZE + Reward.NEW_BARRACK_CNT] * 10
            re1[i] += totalObs.ob1.reward[i * REWARD_SIZE + Reward.NEW_LIGHT_CNT] * 2
            re1[i] += totalObs.ob1.reward[i * REWARD_SIZE + Reward.NEW_RANGED_CNT] * 4
            re1[i] += totalObs.ob1.reward[i * REWARD_SIZE + Reward.NEW_HEAVY_CNT] * 8
            re1[i] += totalObs.ob1.reward[i * REWARD_SIZE + Reward.NEW_HIT_CNT]
            re1[i] += -totalObs.ob1.reward[i * REWARD_SIZE + Reward.VICTORY_SIDE] * 1000
            re2[i] += totalObs.ob2.reward[i * REWARD_SIZE + Reward.NEW_NET_INCOME]
            re2[i] += totalObs.ob2.reward[i * REWARD_SIZE + Reward.NEW_WORKER_CNT]
            re2[i] += totalObs.ob2.reward[i * REWARD_SIZE + Reward.NEW_BARRACK_CNT] * 10
            re2[i] += totalObs.ob2.reward[i * REWARD_SIZE + Reward.NEW_LIGHT_CNT] * 2
            re2[i] += totalObs.ob2.reward[i * REWARD_SIZE + Reward.NEW_RANGED_CNT] * 4
            re2[i] += totalObs.ob2.reward[i * REWARD_SIZE + Reward.NEW_HEAVY_CNT] * 8
            re2[i] += totalObs.ob2.reward[i * REWARD_SIZE + Reward.NEW_HIT_CNT]
            re2[i] += totalObs.ob2.reward[i * REWARD_SIZE + Reward.VICTORY_SIDE] * 1000

        re1 = re1.to(self.device)
        re2 = re2.to(self.device)
        return (ob1, ob2),  (re1, re2), isEnd, ""


if __name__ == "__main__":
    env = VecEnv(128, torch.device("cpu"))
    # ob = env.reset(0, False, True, 1, 5, 5, 100)
    baseCoord = [None] * env.num_workers
    ob = env.reset(0, False, True, 0, 1, 1, 100)
    action1 = torch.zeros((env.num_workers, len(ACTION_SIZE), GAME_H, GAME_W)).type(torch.int8)
    for w in range(env.num_workers):
        for y in range(GAME_H):
            for x in range(GAME_W):
                if ob[0][w, ObPlane.IS_BASE, y, x] != 0 and ob[0][w, ObPlane.OWNER_1, y, x] != 0:
                    action1[w, ActionPlane.ACTION, y, x] = ActionType.PRODUCE
                    action1[w, ActionPlane.PRODUCE_TYPE_PARAM, y, x] = ObjType.WORKER
                    action1[w, ActionPlane.PRODUCE_DIRECTION_PARAM, y, x] = 2
                    baseCoord[w] = (y, x)
    for w in range(env.num_workers):
        for y in range(baseCoord[w][0], GAME_H):
            action1[w, ActionPlane.ACTION, y, baseCoord[w][1]] = ActionType.MOVE
            action1[w, ActionPlane.MOVE_PARAM, y, baseCoord[w][1]] = 2
            if ob[0][w, ObPlane.IS_BASE, y, baseCoord[w][1]] != 0 and ob[0][w, ObPlane.OWNER_2, y, baseCoord[w][1]] != 0:
                action1[w, ActionPlane.ACTION, y - 1, baseCoord[w][1]] = ActionType.ATTACK
                action1[w, ActionPlane.RELATIVE_ATTACK_POSITION, y - 1, baseCoord[w][1]] = 7 * 4 + 3
        action1[w, ActionPlane.ACTION, baseCoord[w][0], baseCoord[w][1]] = ActionType.PRODUCE

    action2 = torch.zeros((env.num_workers, len(ACTION_SIZE), GAME_H, GAME_W)).type(torch.int8)
    for i in range(5 + 2 * 32 + 60 * 2): # 5 step produce, 2*32 step move, 60 * 2 step attack
        o, r, isEnd, _ = env.step(action1, action2)
    plt.imshow(o[1][-1, ObPlane.IS_BASE])
    plt.show()
