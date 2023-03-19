import socket

IP = "127.0.0.1"
PORT = 5555

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

sock.sendto(b"Hello World", (IP, PORT))
