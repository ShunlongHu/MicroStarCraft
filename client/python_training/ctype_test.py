from ctypes import *
from game_types import *
import numpy as np
import torch
obj = cdll.LoadLibrary("D:/repo/rts/cmake-build-release/server/engine/rts_engine_shared.dll")
initParam = InitParam(c_int(5), c_int(5))

data = np.array([1, 2, 3, 4, 5, 6, 7, 8]).ctypes.data_as(POINTER(c_byte))
action = Action(data, c_int(8))
obj.Init(initParam)

obj.Step.restype = TotalObservation

obs = obj.Step(TotalAction(action, action))

print(f"length1={obs.ob1.size}")
for i in range(obs.ob1.size):
    print(obs.ob1.data[i])

print(f"length2={obs.ob2.size}")
for i in range(obs.ob2.size):
    print(obs.ob2.data[i])

print("time = ", obs.time)
print("isEnd = ", obs.isEnd)
print("winningSide = ", obs.winningSide)
print("size = ", sizeof(obs))

if __name__ == "main":
    pass

    # out_buf = so_obj.Step(action)
    # print(out_buf)

