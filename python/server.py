from socket import *

serverPort = 12000
serverSocket = socket(AF_INET, SOCK_DGRAM)  # Use SOCK_DGRAM for UDP
serverSocket.bind(("", serverPort))
print(f"Server is running on port {serverPort}")

while True:
    message, clientAddress = serverSocket.recvfrom(
        2048
    )  # Receive message and client address
    print(f"Received message: {message.decode()} from {clientAddress}")
    serverSocket.sendto(
        message, clientAddress
    )  # Send the modified message back to the client
