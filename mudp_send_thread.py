import socket
import time
from math import ceil
import threading


M_Group = "239.1.2.3"
LOCALHOST = socket.gethostbyname(socket.gethostname())
PORT = 1234

# UDP Setting
udp_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
udp_sock.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_IF, socket.inet_aton(LOCALHOST))
udp_sock.setsockopt(
    socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP,
    socket.inet_aton(M_Group) + socket.inet_aton(LOCALHOST)
)
# 自分の送ったマルチキャストパケットを自身で受け取らない
udp_sock.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_LOOP, 0)


# TCP Setting
tcp_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
tcp_sock.settimeout(5)
tcp_sock.bind((LOCALHOST, PORT))
tcp_sock.listen(10)


start = 0
FILE_NAME = 'send_100.txt'
file_d = ""
len_d = 0

with open(FILE_NAME, 'r') as f:
    file_d = f.read()
    len_d = len(file_d)


count=0
# chunks = [data[i:i+1000] for i in range(0, len(data), 1000)]
chunks = [file_d[i:i+10] for i in range(0, len_d, 10)]
max_seq = str(ceil(len_d / 10))

start = time.time()
for chunk in chunks:
	count+=1
	chunk = max_seq + ":" + str(count) + ":" + chunk

	# udp_sock.sendto(chunk.encode(), (M_Group, PORT))
	# time.sleep(0.001)
	
	# 2個パケットロスを発生
	if count == 3 or count == 6: 
		continue
	else:
		print(chunk)
		udp_sock.sendto(chunk.encode(), (M_Group, PORT))
		time.sleep(0.001)



# データ受信ループ関数
def recv_client(sock, addr):
	recv_seq_ary = []

	while sock:
		res_data = sock.recv(1024)

		recv_seq_ary = res_data.decode().split(",")
		recv_seq_ary.pop(-1)

		recv_seq_ary = list(map(int, recv_seq_ary))
		print(recv_seq_ary)

		for i in recv_seq_ary:
			# chunk = data[(i-1)*1000:(i-1)*1000+1000]
			chunk = file_d[(i-1)*10:(i-1)*10+10]
			chunk = str(i) + ":" + chunk
		
			print(chunk)
			sock.send(chunk.encode())

		sock.close()
		print(time.time() - start)
		print()
		sock=None


while True:
	client, addr = tcp_sock.accept()
	print("-----------------------")
	
	thread = threading.Thread(target=recv_client, args=(client, addr))

	thread.start()
