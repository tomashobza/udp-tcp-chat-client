#ifndef TYPES_H
#define TYPES_H

#include <cstdint>
#include <string>
#include <vector>
#include "constants.hpp"

enum MessageTypeEnum : uint8_t
{
    CONFIRM = 0x00,
    REPLY = 0x01,
    AUTH = 0x02,
    JOIN = 0x03,
    MSG = 0x04,
    ERR = 0xFE,
    BYE = 0xFF,
    UNKNOWN = 0x05
};
typedef enum MessageTypeEnum MessageType;

typedef uint16_t MessageID;

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
    MessageID ref_id;
    uint8_t result;
    std::string username;
    std::string display_name;
    std::string password;
    std::string channel_id;
    std::string contents;
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

typedef std::vector<PollResult> PollResults;

typedef enum
{
    CMD_AUTH,
    CMD_JOIN,
    CMD_RENAME,
    CMD_MSG,
    CMD_HELP
} CommandType;

typedef struct
{
    CommandType type;
    std::vector<std::string> args;
} Command;

#endif // TYPES_H