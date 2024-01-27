import matplotlib.pyplot as plt
import torch
from vec_env import *
from torch.distributions.categorical import Categorical
import torch.nn.functional as F


def sample(action: torch.Tensor, mask: torch.Tensor) -> torch.Tensor:
    neg_inf = -1e6
    permutedAction = action.permute(0, 2, 3, 1)
    logits = torch.where(mask.permute(0, 2, 3, 1) == 1, permutedAction, neg_inf)
    resultantAction = torch.zeros([*permutedAction.size()[:-1], 0])
    lastDim = 0
    for i in ACTION_SIZE:
        dist = Categorical(logits=logits[:, :, :, lastDim:lastDim + i])
        resultantAction = torch.concat([resultantAction, dist.sample().unsqueeze(-1)], dim=-1)
        lastDim += i
    return resultantAction.permute(0, 3, 1, 2)


if __name__ == "__main__":
    env = VecEnv(1, torch.device("cpu"), 0, False, True, 0, 5, 5, 100)
    ob, mask = env.reset()
    torch.manual_seed(0)
    for i in range(1000):
        action1 = torch.rand((env.num_workers, sum(ACTION_SIZE), GAME_H, GAME_W))
        action2 = torch.rand((env.num_workers, sum(ACTION_SIZE), GAME_H, GAME_W))
        o, mask, r, isEnd, t = env.step(sample(action1, mask[0]), sample(action2, mask[1]))
        plt.imshow(o[0][0, ObPlane.OBSTACLE])
        print(r[0][0], r[1][0], isEnd[0])
        ax = plt.gca()
        ax.set_xticks(np.arange(0, 32, 1))
        ax.set_yticks(np.arange(0, 32, 1))
        ax.grid(color='w', linestyle='-', linewidth=1)
        plt.savefig(f"ob/{i}.png")
