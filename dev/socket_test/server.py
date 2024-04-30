import socket
import json

#server accepts request and prints out the array of controller inputs but haven't tested with controller yet.
#it sends back "accepted request" if there's data that's been sent else request DENIED.

HOST = "127.0.0.1" #Standard loopback interface address(localhost)
PORT = 65432 #port to listen on (non-privileged ports are > 1023)

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.bind((HOST, PORT))
    s.listen()
    conn, addr = s.accept()
    with conn:
        print(f"Connected by {addr}")
        while True:
            data = conn.recv(1024)
            if not data:
                message = "request DENIED >:(!!"
                conn.sendall(message.encode())
                break
            received_data = json.loads(data.decode())
            message = "Accepted request"
            print(received_data)
            conn.sendall(message.encode())