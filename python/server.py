from socket import *
import threading


def udp_server():
    UDPserverPort = 12000
    UDPserverSocket = socket(AF_INET, SOCK_DGRAM)
    UDPserverSocket.bind(("", UDPserverPort))
    print(f"UDP Server is running on port {UDPserverPort}")

    while True:
        message, clientAddress = UDPserverSocket.recvfrom(2048)
        changedMessage = message.upper()
        print(f"Received message: {message.decode()} from {clientAddress}")
        UDPserverSocket.sendto(changedMessage, clientAddress)


def tcp_server():
    TCPserverPort = 12001
    TCPserverSocket = socket(AF_INET, SOCK_STREAM)
    TCPserverSocket.bind(("", TCPserverPort))
    TCPserverSocket.listen(1)
    print(f"TCP Server is running on port {TCPserverPort}")

    while True:
        connectionSocket, addr = TCPserverSocket.accept()
        message = connectionSocket.recv(2048).decode()
        changedMessage = message.upper()
        print(f"Received message: {message} from {addr}")
        connectionSocket.send(changedMessage.encode())
        connectionSocket.close()


# Create threads for UDP and TCP servers
udp_thread = threading.Thread(target=udp_server)
tcp_thread = threading.Thread(target=tcp_server)

# Start the server threads
udp_thread.start()
tcp_thread.start()
