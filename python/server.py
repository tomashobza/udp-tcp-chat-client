from socket import *
import threading
import time
import random
from termcolor import cprint


class MessageType:
    CONFIRM = 0
    REPLY = 1
    AUTH = 2
    JOIN = 3
    MSG = 4
    ERR = 0xFE
    BYE = 0xFF

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


def udp_server():
    ref_msg_id = 0
    msg_id = 0
    UDPserverPort = 4567
    UDPserverSocket = socket(AF_INET, SOCK_DGRAM)
    UDPserverSocket.bind(("", UDPserverPort))
    print(f"\033[93mUDP Server is running on port {UDPserverPort}\033[0m")

    threadik = None

    while True:
        message, clientAddress = UDPserverSocket.recvfrom(2048)
        message_type = message[0]
        cprint(f"UDP: Received message: {message} from {clientAddress}", "cyan")

        print(f"Message type: {MessageType(message_type)}")

        # confirm message
        UDPserverSocket.sendto(
            bytes([MessageType.CONFIRM, message[1], message[2]]), clientAddress
        )
        cprint(
            f"UDP: Sent CONFIRM {bytes([MessageType.CONFIRM, message[1], message[2]])} to {clientAddress}",
            "magenta",
        )

        inc_msg_id = message[1] << 8 | message[2]

        # AUTH state
        if message_type == MessageType.AUTH:
            time.sleep(1)

            reply = 0
            if (
                message[3:] == b"a\x00a\x00a\x00"
                or message[3:] == b"aa\x00aa\x00aa\x00"
            ):
                reply = 1

            UDPserverSocket.sendto(
                bytes([MessageType.REPLY, 0, msg_id, reply, message[1], message[2]])
                + b"cecky",
                clientAddress,
            )
            msg_id += 1 if ref_msg_id < inc_msg_id else 0
            cprint(
                f"UDP: Sent REPLY {str(bytes([MessageType.REPLY, 0, msg_id, reply, message[1], message[2]]))} to {clientAddress}",
                "yellow",
            )
        elif message_type == MessageType.JOIN:
            time.sleep(1)

            UDPserverSocket.sendto(
                bytes([MessageType.REPLY, 0, msg_id, 1, message[1], message[2]])
                + b"cecky",
                clientAddress,
            )
            msg_id += 1 if ref_msg_id < inc_msg_id else 0
            cprint(f"UDP: Sent REPLY to {clientAddress}", "yellow")
        elif message_type == MessageType.MSG:
            time.sleep(1)

            UDPserverSocket.sendto(bytes([6, 2, 3, 4, 5, 6]), clientAddress)
            msg_id += 1 if ref_msg_id < inc_msg_id else 0
            cprint(f"UDP: Sent message to {clientAddress}", "yellow")

        if message_type != MessageType.CONFIRM:
            ref_msg_id = inc_msg_id


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
