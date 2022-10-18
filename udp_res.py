import ubinascii as ub
from usocket import socket, AF_INET, SOCK_DGRAM

PORT = 8086

# ソケットを用意
s = socket(AF_INET, SOCK_DGRAM)
# バインドしておく
s.bind(('0.0.0.0', PORT))

while True:
    # 受信
    msg, address = s.recvfrom(1024)
    print(f"message: {ub.unhexlify(msg)}\nfrom: {address}")


# ソケットを閉じておく
s.close()