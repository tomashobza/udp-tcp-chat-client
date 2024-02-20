#include "./utils.h"

int create_socket()
{
    int client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_socket < 0)
    {
        perror("ERROR in socket");
        exit(EXIT_FAILURE);
    }
    return client_socket;
}

struct sockaddr_in get_server_address(const char *server_hostname, const int port_number)
{
    struct sockaddr_in server_address; // initialize the server address

    struct hostent *server = gethostbyname(server_hostname); // get the server by name
    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host as %s\n", server_hostname);
        exit(EXIT_FAILURE);
    }

    memset((char *)&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    memcpy((char *)&server_address.sin_addr.s_addr, (char *)server->h_addr, server->h_length);
    server_address.sin_port = htons(port_number);

    return server_address;
}

char *get_message()
{
    char *buf = malloc(sizeof(char) * 2048);
    printf("Enter message: ");
    fgets((char *)buf, 2048, stdin);
    return buf;
}