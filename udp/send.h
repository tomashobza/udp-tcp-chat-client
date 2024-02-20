#ifndef UDP_SEND_H
#define UDP_SEND_H

#include "utils.h"

#define h_addr h_addr_list[0] /* for backward compatibility */
#define MAX_MESSAGE_SIZE 1590

// Message field types
typedef enum
{
    CONFIRM = (uint8_t)0x00,
    REPLY = (uint8_t)0x01,
    AUTH = (uint8_t)0x02,
    JOIN = (uint8_t)0x03,
    MSG = (uint8_t)0x04,
    ERR = (uint8_t)0xFE,
    BYE = (uint8_t)0xFF
} MessageTypeEnum;
typedef uint8_t MessageType;

typedef uint16_t MessageID;
typedef uint8_t MessageResult;
typedef uint8_t MessageEnd;

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
    MessageResult res;
    MessageID ref_id;
    char *content;
} Message_REPLY;

void confirm()

#endif // UDP_SEND_H