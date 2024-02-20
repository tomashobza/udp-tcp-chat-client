// Can't live without io and memory allocation
#include <stdio.h>
#include <stdlib.h>
// Not necessary but recomended for portability in some systems
#include <sys/types.h>
// The socket lib
#include <sys/socket.h>
// For closing
#include <unistd.h>
// We are going to need this to work with the addresses
#include <string.h>
// This is where getnameinfo is
#include <netdb.h>
#include <arpa/inet.h>

#include "udp/utils.h"
#include "udp/send.h"

int main(void)
{
  // printf("%lu\n", sizeof(MessageType));

  const char *server_hostname = "localhost";
  const int port_number = 12000;

  // Create the socket
  int client_socket = create_socket();
  printf("Socket created %d\n", client_socket);
  printf("\n");

  // Get the server address
  struct sockaddr_in server_address = get_server_address(server_hostname, port_number);
  // size_t server_address_len = sizeof(server_address);

  int bytes_sent = send_AUTH(client_socket, &server_address, "username", "display_name", "password", 0);
  printf("Bytes sent: %d\n", bytes_sent);

  // // Get the message from the user
  // char *message = get_message();

  // // Send the message to the server
  // ssize_t bytestx = sendto(client_socket, message, strlen(message), 0, (struct sockaddr *)&server_address, server_address_len);
  // if (bytestx < 0)
  // {
  //   perror("ERROR in sendto");
  //   exit(EXIT_FAILURE);
  // }

  // printf("Bytes sent: %ld\n", bytestx);
  // printf("To: %s:%d\n", inet_ntoa(server_address.sin_addr), ntohs(server_address.sin_port));
  // printf("\n");

  // void *response = malloc(sizeof(char) * 2048);

  // // Wait for the response from the server
  // bytestx = recvfrom(client_socket, response, sizeof(char) * 2048, 0, (struct sockaddr *)&server_address, ((socklen_t *)&server_address_len));

  // if (bytestx < 0)
  // {
  //   perror("ERROR in recvfrom");
  //   exit(EXIT_FAILURE);
  // }

  // printf("Bytes received: %ld\n", bytestx);
  // printf("From: %s:%d\n", inet_ntoa(server_address.sin_addr), ntohs(server_address.sin_port));
  // printf("Message: %s\n", (char *)response);
  // printf("\n");

  // free(message);
  // free(response);
  close(client_socket);

  return 0;
}
