#ifndef TYPES_H
#define TYPES_H

enum MessageTypeEnum : uint8_t
{
    CONFIRM = 0x00,
    REPLY = 0x01,
    AUTH = 0x02,
    JOIN = 0x03,
    MSG = 0x04,
    ERR = 0xFE,
    BYE = 0xFF
};
typedef enum MessageTypeEnum MessageType;

typedef uint16_t MessageID;

#define BEG_OFFSET (sizeof(MessageType) + sizeof(MessageID))
#define STR_OFFSET sizeof(uint8_t)

struct IMessage
{
    MessageType type;
    MessageID id;
    std::vector<uint8_t> data;
};

typedef struct IMessage Message;

struct ConfirmWaiter
{
    uint8_t tries_left;
    int time_left;
    MessageID id;
    std::vector<uint8_t> data;
};

#endif // TYPES_H