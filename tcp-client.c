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
#include <sys/_endian.h>

int main(void)
{
    int client_socket;
    const char *server_hostname = "localhost";
    const int port_number = 12001;

    // SOCKET

    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("ERROR in socket");
        exit(EXIT_FAILURE);
    }

    printf("Socket created %d\n", client_socket);

    // SERVER ADDRESS

    struct hostent *server = gethostbyname(server_hostname);
    struct sockaddr_in server_address;

    printf("Server: %s %d\n", server->h_name, server->h_addrtype);

    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host as %s\n", server_hostname);
        exit(EXIT_FAILURE);
    }

    bzero((char *)&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&server_address.sin_addr.s_addr, server->h_length);
    server_address.sin_port = htons(port_number);

    // CONNECT

    if (connect(client_socket, (const struct sockaddr *)&server_address, sizeof(server_address)) != 0)
    {
        perror("ERROR in connect");
        exit(EXIT_FAILURE);
    }

    for (;;)
    {
        void *buf = calloc(2048, sizeof(char));
        void *buf2 = calloc(2048, sizeof(char));

        printf("Enter message: ");
        fgets((char *)buf, 2048, stdin);

        if (strcmp((char *)buf, "exit\n") == 0)
        {
            break;
        }

        ssize_t bytestx = write(client_socket, buf, 2048 * sizeof(char));
        if (bytestx < 0)
        {
            perror("ERROR in sendto");
            exit(EXIT_FAILURE);
        }

        printf("Bytes sent: %ld\n", bytestx);

        bytestx = read(client_socket, buf2, 2048);

        if (bytestx < 0)
        {
            perror("ERROR in recvfrom");
            exit(EXIT_FAILURE);
        }

        printf("Bytes received: %ld\n", bytestx);
        printf("Message: %s\n", (char *)buf2);

        if (buf != NULL)
        {
            free(buf);
        }
        if (buf2 != NULL)
        {
            free(buf2);
        }
    }

    free(buf2);
    close(client_socket);

    return 0;
}
