from strategy import *
import run_code_gen
import logging
import time
import random
import grpc
import message_pb2
import message_pb2_grpc
from message_pb2 import *

def run(ip, port, side):
    role = Role.OBSERVER
    if side == "PLAYER_A":
        role = Role.PLAYER_A
    if side == "PLAYER_B":
        role = Role.PLAYER_B
    if role == Role.OBSERVER:
        print("Side must be either 'PLAYER_A' or 'PLAYER_B'")
        return
    with grpc.insecure_channel(f"{ip}:{port}") as channel:
        stub = message_pb2_grpc.RtsStub(channel)
        call = stub.ConnectPlayer(iter([PlayerRequest(command=STEP, role=role)]))
        time.sleep(0.1)

        while True:
            for response in call:
                if response == grpc.aio.EOF:
                    break
                print(f"Received: {response}")
            call = stub.ConnectPlayer(iter([PlayerRequest(command=STEP, role=role)]))


if __name__ == "__main__":
    logging.basicConfig()
    run("127.0.0.1", "8000", "PLAYER_B")