// Can't live without io and memory allocation
#include <stdio.h>
#include <stdlib.h>
// Not necessary but recomended for portability in some systems
// #include <sys/types.h>
// The socket lib
#include <sys/socket.h>
// For closing
// #include <unistd.h>
// We are going to need this to work with the addresses
#include <string.h>
// This is where getnameinfo is
// #include <netdb.h>

int main(void)
{
  int client_tocket;
  int server;

  if ((client_tocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    perror("ERROR in socket");
    exit(EXIT_FAILURE);
  }

  printf("Socket created %d\n", client_tocket);

  server = gethostbyname();
}
