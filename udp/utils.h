#ifndef UDP_UTILS_H
#define UDP_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>

// Message field types
typedef enum : uint8_t
{
    CONFIRM = (uint8_t)0x00,
    REPLY = (uint8_t)0x01,
    AUTH = (uint8_t)0x02,
    JOIN = (uint8_t)0x03,
    MSG = (uint8_t)0x04,
    ERR = (uint8_t)0xFE,
    BYE = (uint8_t)0xFF
} MessageType;

typedef uint16_t MessageID;
typedef uint8_t MessageResult;

// Message structures based on the type
typedef struct
{
    MessageType type;
    MessageID msg_id;
} Message_CONFIRM;

typedef struct
{
    MessageType type;
    MessageID msg_id;
    char *message;
} Message_REPLY;

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