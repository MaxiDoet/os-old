import socket
import time

PORT = 5000

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

for i in range(10):
	sock.sendto(bytes([i]), ("127.0.0.1", PORT))
	time.sleep(1)
