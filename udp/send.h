#ifndef UDP_SEND_H
#define UDP_SEND_H

#include "utils.h"

#define h_addr h_addr_list[0] /* for backward compatibility */
#define MAX_MESSAGE_SIZE 1590

#include <string.h>
#include <stdlib.h>

// Message field types
typedef uint8_t MessageType;

#define CONFIRM (MessageType)0x00
#define REPLY (MessageType)0x01
#define AUTH (MessageType)0x02
#define JOIN (MessageType)0x03
#define MSG (MessageType)0x04
#define ERR (MessageType)0xFE
#define BYE (MessageType)0xFF

typedef uint16_t MessageID;
typedef uint8_t MessageResult;
typedef uint8_t MessageEnd;

int send_AUTH(int sockfd, struct sockaddr_in *server, char *username, char *display_name, char *password, MessageID msg_id);

int send_JOIN(int sockfd, struct sockaddr_in *server, char *channel_id, char *display_name, MessageID msg_id);

#endif // UDP_SEND_H