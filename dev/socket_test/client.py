import socket
import json
from Controller import *


#so far this has been tested to send array of inputs to server but haven't tested with controller yet.
#it sends to server and receives a response message, "received request accepted", until stopping connection
#need to test with controller to see if server has anything besides an array of zeros.
#need to add signal handling for control-c so it exits with a zero status code.

HOST = socket.gethostbyname(socket.gethostname())
PORT = 65432

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