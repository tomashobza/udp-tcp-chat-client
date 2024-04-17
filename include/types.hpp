/**
 * @file types.hpp
 * @author Tomáš Hobza (xhobza03)
 * @brief Combined types for the project
 * @date 2024-03-31
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef TYPES_H
#define TYPES_H

#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>
#include "constants.hpp"

/**
 * @brief Enum for message types
 *
 */
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
/**
 * @brief Enum for message types
 */
typedef enum MessageTypeEnum MessageType;

/**
 * @brief Message ID
 *
 */
typedef uint16_t MessageID;

/**
 * @brief Complete Message structure
 */
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
/**
 * @brief Message structure
 *
 */
typedef struct IMessage Message;

/**
 * @brief Confirm waiter structure for waiting for confirmations
 *
 */
struct ConfirmWaiter
{
    uint8_t tries_left;
    long long expiration;
    MessageID id;
    std::vector<uint8_t> data;
};

/**
 * @brief Enum for protocol types
 *
 */
typedef enum
{
    TCP,
    UDP
} Protocol;

/**
 * @brief Arguments structure
 *
 */
typedef struct
{
    Protocol type;
    std::string hostname;
    uint16_t port;
    uint16_t timeout;
    uint8_t max_tries;
} Args;

/**
 * @brief Enum for states of the Automata
 *
 */
typedef enum
{
    S_START,
    S_AUTH,
    S_OPEN,
    S_ERROR,
    S_END
} State;

/**
 * @brief Enum for the result types of message polling
 *
 */
typedef enum
{
    USER,
    SERVER,
} PollResultType;

/**
 * @brief Poll result structure for message polling
 *
 */
typedef struct
{
    PollResultType type;
    Message message;
} PollResult;

/**
 * @brief Poll results vector
 *
 */
typedef std::vector<PollResult> PollResults;

/**
 * @brief Command type enum
 *
 */
typedef enum
{
    CMD_AUTH,
    CMD_JOIN,
    CMD_RENAME,
    CMD_MSG,
    CMD_HELP
} CommandType;

/**
 * @brief Command structure
 *
 */
typedef struct
{
    CommandType type;
    std::vector<std::string> args;
} Command;

#endif // TYPES_H