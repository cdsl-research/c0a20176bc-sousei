import socket
import time
import asyncio
import selectors


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
udp_sock.bind(('', PORT))
udp_sock.settimeout(3)
udp_sock.setblocking(False)

# udp_sock.connect((M_Group, PORT)) 


file_data = ""
len_data = 0
FILE_NAME = 'send.txt'
count = 0


with open(FILE_NAME, 'r') as f:
    file_data = f.read()
    len_data = len(file_data)


def send_tcp(addr, port, ary):
    
    try: 
        # packet loss data resend
        tcp_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        tcp_sock.connect((addr, port))
        
        print("start to send by tcp...")
        for i in ary:
            chunk = file_data[i*10:i*10+10]
            chunk = str(i) + ":" + chunk
            print(chunk)
            
            tcp_sock.send(chunk.encode())

        print(f'{addr}:{port} sending done!\n')
        tcp_sock.close()

    except Exception as e:
        print ('=== エラー内容 ===')
        print ('type:' + str(type(e)))
        print ('args:' + str(e.args))
        print ('message:' + e.message)
        print ('e自身:' + str(e))
       


def recv_loss_packet():    
    print("time to wait for response...")
    try:
        res_data, (src_ip, src_port) = udp_sock.recvfrom(1024)        
        
        # packet loss seq split
        recv_seq_ary = res_data.decode().split(",")
        recv_seq_ary.pop(-1)

        recv_seq_ary = list(map(int, recv_seq_ary))
        send_tcp(src_ip, src_port, recv_seq_ary)
        
    except:
        print(f"failure!")
        loop.stop()
    finally:
        count = count + 1

        if count == 4:
            loop.stop()



start = time.time()
# データを指定バイトごとに分割
chunks = [file_data[i:i+10] for i in range(0, len_data, 10)]

for chunk in chunks:
    count+=1
    chunk = str(count) + ":" + chunk

    # 2個パケットロスを発生
    if count == 3 or count == 6: 
        continue
    else:
        print(chunk)
        udp_sock.sendto(chunk.encode(), (M_Group, PORT))
        time.sleep(0.05)

print("-------------------------------------")
count = 0

selector = selectors.SelectSelector()
loop = asyncio.SelectorEventLoop(selector)
asyncio.set_event_loop(loop)

loop.add_reader(udp_sock.fileno(), recv_loss_packet)

try:
    loop.run_forever()
finally:
    print(time.time() - start, count)
    loop.close()

