#ifndef UDP_UTILS_H
#define UDP_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>

/**
 * @brief Create an internet UDP socket. If the socket creation fails, the program will exit with EXIT_FAILURE.
 *
 * @return int - The socket file descriptor
 */
int create_socket();

/**
 * @brief Get the server address object from the server hostname and port number.
 *
 * @param server_hostname - The server hostname
 * @param port_number - The port number
 * @return struct sockaddr_in - The server address
 */
struct sockaddr_in get_server_address(const char *server_hostname, const int port_number);

/**
 * @brief Get the message from the user.
 *
 * @return char* - The message
 */
char *get_message();

#endif // UDP_UTILS_H