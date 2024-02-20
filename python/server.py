from socket import *
import threading


def udp_server():
    UDPserverPort = 12000
    UDPserverSocket = socket(AF_INET, SOCK_DGRAM)
    UDPserverSocket.bind(("", UDPserverPort))
    print(f"\033[93mUDP Server is running on port {UDPserverPort}\033[0m")

    while True:
        message, clientAddress = UDPserverSocket.recvfrom(2048)
        changedMessage = message.upper()
        print(
            f"\033[94mUDP: Received message: {message.decode()} from {clientAddress}\033[0m"
        )
        UDPserverSocket.sendto(changedMessage, clientAddress)


def tcp_server():
    TCPserverPort = 12001
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
            print(f"\033[94mTCP: Received message: {message} from {addr}\033[0m")
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
