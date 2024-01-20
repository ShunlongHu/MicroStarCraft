from strategy import *
import run_code_gen
import logging
import time
import random
import grpc
import message_pb2
import message_pb2_grpc
from message_pb2 import *
from typing import Iterable

role = Role.OBSERVER
state = GameState()


def generate_action() -> Iterable[PlayerRequest]:
    global role
    global state
    lastState = GameState()
    yield PlayerRequest(command=RESET, role=role)
    while True:
        if lastState != state:
            lastState = state
            action = Act(state)
            retVal = PlayerRequest(command=STEP, role=role)
            for k, v in action.items():
                curAction = Action(id=k, action=v.action, produceType=v.produceType, targetX=v.target.x, targetY=v.target.y)
                retVal.actions.append(curAction)
            yield retVal
        time.sleep(0.0001)

def run(ip, port, side):
    global state
    global role
    if side == "PLAYER_A":
        role = Role.PLAYER_A
    if side == "PLAYER_B":
        role = Role.PLAYER_B
    if role == Role.OBSERVER:
        print("Side must be either 'PLAYER_A' or 'PLAYER_B'")
        return
    with grpc.insecure_channel(f"{ip}:{port}") as channel:
        stub = message_pb2_grpc.RtsStub(channel)
        call = stub.ConnectPlayer(generate_action())
        time.sleep(0.1)

        while True:
            time.sleep(0.00001)
            for response in call:
                if response == grpc.aio.EOF:
                    break
                state = ByteStreamToGameState(response.data)


if __name__ == "__main__":
    logging.basicConfig()
    run("127.0.0.1", "8000", "PLAYER_B")
