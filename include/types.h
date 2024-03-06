#ifndef TYPES_H
#define TYPES_H

#include <cstdint>
#include <string>
#include <vector>

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

/// Message contents ///
typedef struct
{
    MessageID ref_id;
} ConfirmMessage;

typedef struct
{
    MessageID ref_id;
    uint8_t reply;
    std::string contents;
} ReplyMessage;

typedef struct
{
    std::string username;
    std::string display_name;
    std::string password;
} AuthMessage;

typedef struct
{
    std::string channel_id;
    std::string display_name;
} JoinMessage;

typedef struct
{
    std::string display_name;
    std::string contents;
} MsgMessage;

typedef struct
{
    std::string display_name;
    std::string contents;
} ErrMessage;

typedef struct
{
} ByeMessage;

/// /////////////////// ///

struct IMessage
{
    MessageType type;
    MessageID id;
    // union
    // {
    //     ConfirmMessage confirm;
    //     ReplyMessage reply;
    //     AuthMessage auth;
    //     JoinMessage join;
    //     MsgMessage msg;
    //     ErrMessage err;
    //     ByeMessage bye;
    // } contents;
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

typedef enum
{
    TCP,
    UDP
} Protocol;

typedef struct
{
    Protocol type;
    std::string hostname;
    uint16_t port;
    uint16_t timeout;
    uint8_t max_tries;
} Args;

typedef enum
{
    S_START,
    S_AUTH,
    S_OPEN,
    S_ERROR,
    S_END
} State;

typedef enum
{
    USER,
    SERVER,
} PollResultType;

typedef struct
{
    PollResultType type;
    Message message;
} PollResult;

#endif // TYPES_H