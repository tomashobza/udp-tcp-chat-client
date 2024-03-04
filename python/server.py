from socket import *
import threading
import time
import random


class MessageType:
    def __init__(self, enum):
        self.enum = enum

    def __repr__(self):
        if self.enum == 0:
            return "\033[95mCONFIRM\033[0m"
        elif self.enum == 1:
            return "\033[93mREPLY\033[0m"
        elif self.enum == 2:
            return "\033[92mAUTH\033[0m"
        elif self.enum == 3:
            return "\033[95mJOIN\033[0m"
        elif self.enum == 4:
            return "\033[96mMSG\033[0m"
        elif self.enum == 0xFE:
            return "\033[91mERR\033[0m"
        elif self.enum == 0xFF:
            return "\033[93mBYE\033[0m"
        else:
            return "\033[91mUNKNOWN\033[0m"


def send_random_message(UDPserverSocket, clientAddress):
    index = 0

    while True:
        time.sleep(random.randint(2, 10))

        response_msg = bytes([4, 0, index]) + b"cicinka" + b"\0" + b"\0"
        print(
            f"\033[92mRandom UDP: Sending message: {response_msg} to {clientAddress}\033[0m"
        )

        index += 1

        UDPserverSocket.sendto(response_msg, clientAddress)


class MessageType:
    def __init__(self, enum):
        self.enum = enum

    def __repr__(self):
        if self.enum == 0:
            return "CONFIRM"
        elif self.enum == 1:
            return "REPLY"
        elif self.enum == 2:
            return "AUTH"
        elif self.enum == 3:
            return "JOIN"
        elif self.enum == 4:
            return "MSG"
        elif self.enum == 0xFE:
            return "ERR"
        elif self.enum == 0xFF:
            return "BYE"
        else:
            return "UNKNOWN"


def udp_server():
    UDPserverPort = 4567
    UDPserverSocket = socket(AF_INET, SOCK_DGRAM)
    UDPserverSocket.bind(("", UDPserverPort))
    print(f"\033[93mUDP Server is running on port {UDPserverPort}\033[0m")

    threadik = None

    while True:
        message, clientAddress = UDPserverSocket.recvfrom(2048)

        if threadik is None:
            # start a new thread to send a random message to the client
            threadik = threading.Thread(
                target=send_random_message, args=(UDPserverSocket, clientAddress)
            )
            threadik.start()
        elif message[0] == 0xFF:
            # stop the thread if the client sends a BYE message
            threadik.join()
            threadik = None

        changedMessage = message.upper()
        # write out the first byte of the message as a number, two next bytes as another number and the rest of the message as a string separated by bytes of value 0
        type = MessageType(message[0])
        msg = ",".join(message[3:].decode().split("\0"))
        print(f"Received: {message}")
        print(
            f"\033[94mUDP: Received message:\n\t{type}:{(message[2]<<1)+message[1]}:'{msg}'\n\tfrom {clientAddress}\033[0m"
        )

        # message = b"Hello from server"
        # response_msg = (
        #     bytes([4, message[1], message[2]]) + b"cicinka" + b"\0" + message + b"\0"
        # )
        # print(f"\033[92mUDP: Sending message: {response_msg} to {clientAddress}\033[0m")

        # UDPserverSocket.sendto(response_msg, clientAddress)
        if message[0] == 0:
            continue

        time.sleep(0.4)
        response_msg = bytes([0, 0, (message[2] << 1) + message[1]])
        print(f"\033[92mUDP: Sending message: {response_msg} to {clientAddress}\033[0m")

        UDPserverSocket.sendto(response_msg, clientAddress)


def tcp_server():
    TCPserverPort = 4568
    TCPserverSocket = socket(AF_INET, SOCK_STREAM)
    TCPserverSocket.bind(("", TCPserverPort))
    TCPserverSocket.listen(1)
    print(f"\033[93mTCP Server is running on port {TCPserverPort}\033[0m")

    while True:
        connectionSocket, addr = TCPserverSocket.accept()
        print(f"\033[92mTCP: Connection from {addr}\033[0m")

        while True:
            message = connectionSocket.recv(2048).decode()
            changedMessage = message.upper()
            print(f"\033[94mTCP:\n\tReceived message: {message} from {addr}\033[0m")
            connectionSocket.send(changedMessage.encode())
            # break the while if the user closed
            if not message:
                break

        connectionSocket.close()
        print(f"\033[91mTCP: Connection from {addr} closed\033[0m")


# Create threads for UDP and TCP servers
udp_thread = threading.Thread(target=udp_server)
tcp_thread = threading.Thread(target=tcp_server)

# Start the server threads
udp_thread.start()
tcp_thread.start()
