import socket
import json
from Controller import *
import sys

HOST = "Change to correct IP" #needs to change depending on what ip the server prints
PORT = 5050
try:
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PORT))
        joy = Controller()
        while True:
            try:
                message = json.dumps(joy.read())
                s.sendall(message.encode())
                data = s.recv(1024)
                print(f"Received {data.decode()!r}")
            except Exception as e:
                print("error!", e)
                break
except KeyboardInterrupt:
    sys.exit(0)
