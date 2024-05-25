import socket
import json

#server accepts request and prints out the array of controller inputs but haven't tested with controller yet.
#it sends back "accepted request" if there's data that's been sent else request DENIED.

HOST = socket.gethostbyname(socket.gethostname())
PORT = 5050 #port to listen on (non-privileged ports are > 1023)

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    print(HOST)
    s.bind((HOST, PORT))
    print("------------[STARTING SERVER]------------\nserver is listening....")
    s.listen()
    conn, addr = s.accept()
    print("----------[ACCEPTED CONNECTION]----------")
    try:
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
    except ConnectionResetError:
        print("----------[CLOSING CONNECTION]----------")
        conn.close()
        print("----------[SERVER CLOSED GOODBYE!]----------")