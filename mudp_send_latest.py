import socket
import time


M_Group = "239.1.2.3"
LOCALHOST = socket.gethostbyname(socket.gethostname())
# TO_HOST = "192.168.100.207"
TO_HOST = "192.168.2.104"
PORT = 3333

FILE_NAME = 'send.txt'

# UDP Setting
udp_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
udp_sock.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_IF, socket.inet_aton(LOCALHOST))
udp_sock.setsockopt(
    socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP,
    socket.inet_aton(M_Group) + socket.inet_aton(LOCALHOST)
)
# 自分の送ったマルチキャストパケットを自身で受け取らない
udp_sock.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_LOOP, 0)
udp_sock.bind(('', PORT))
udp_sock.settimeout(4)

# udp_sock.connect((M_Group, PORT)) 


# TCP Setting
tcp_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)


recv_seq_ary = []
count=0

with open(FILE_NAME, 'r') as f:
	data = f.read()

	# データを指定バイトごとに分割
	chunks = [data[i:i+10] for i in range(0, len(data), 10)]

	for chunk in chunks:
		count+=1
		chunk = str(count) + ":" + chunk
		print(chunk)

		udp_sock.sendto(chunk.encode(), (M_Group, PORT))
		time.sleep(0.05)


	# UDP response recv
	# data "2,3,7"
	print("time to wait for response...")
	try:
		res_data, (src_ip, src_port) = udp_sock.recvfrom(1024)
	except:
		print("Timeout!")
		udp_sock.close()
		exit()

	print(f'src_ip: {src_ip}, src_port: {src_port}')
	
	
	# packet loss seq split
	recv_seq_ary = list(map(int, res_data.decode().split(",")))
	print(recv_seq_ary)

	udp_sock.close()


	# packet loss data resend
	tcp_sock.connect((src_ip, src_port))
	
	for i in recv_seq_ary:
		chunk = data[i:i+10]
		chunk = str(i) + ":" + chunk
		print(chunk)
		
		tcp_sock.send(chunk.encode())

	tcp_sock.close()









