from socket import *

serverName = "localhost"
serverPort = 12000
clientSocket = socket(AF_INET, SOCK_DGRAM)  # Create a UDP socket

message = input("Input: ")
clientSocket.sendto(
    message.encode(), (serverName, serverPort)
)  # Encode the message to bytes and send

receivedMessage, serverAddress = clientSocket.recvfrom(2048)  # Receive the response
print(
    f"From server: {receivedMessage.decode()}"
)  # Decode the received bytes to a string

clientSocket.close()  # Close the socket
