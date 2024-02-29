#ifndef POSTMAN_H

#define POSTMAN_H
#include <iostream>
#include <cstring>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdexcept>
#include <unistd.h>
#include <vector>
#include <sys/time.h>
#include "sock.h"
#include <stack>
#include <chrono>

/// TYPES ///

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

#define MSG_MAX_RETRIES 3
#define MSG_TIMEOUT 250

typedef struct
{
    MessageType type;
    MessageID id;
    std::vector<uint8_t> data;
} Message;

/// CLASSES ///

/**
 * @brief A class for sending messages to a server.
 */
class Postman
{
private:
    /** Client side socket file descriptor */
    Sock client_socket;
    /** Server address */
    struct sockaddr_in server_address;
    /** The ID of the next message to be sent. */
    int msg_id = 0;
    /** The ID of the last message received. */
    int ref_msg_id = 0;

    std::vector<uint8_t> last_message;

public:
    std::stack<Message> message_stack;

    /**
     * @brief Construct a new Postman object and create a socket.
     *
     */
    Postman();

    /**
     * @brief Destroy the Postman object and close the socket.
     *
     */
    ~Postman();

    /**
     * @brief Attach to a server by hostname and port number.
     *
     * @param server_hostname - the hostname of the server
     * @param port_number - the port number of the server
     */
    void attach_to_server(const std::string &server_hostname, uint16_t port_number);

    /**
     * @brief Send the AUTH message to the server.
     *
     * @param username - the username
     * @param display_name - the display name
     * @param password - the password
     * @return int - >0 (number of sent B) if successful, -1 if failed
     */
    int authorize(const std::string &username, const std::string &display_name, const std::string &password);

    /**
     * @brief Send the JOIN message to the server.
     *
     * @param channel_id - the channel ID
     * @param DisplayName - the display name
     * @return int - >0 (number of sent B) if successful, -1 if failed
     */
    int join(const std::string &channel_id, const std::string &display_name);

    /**
     * @brief Send the MSG message to the server.
     *
     * @param display_name - the display name
     * @param message_contents - the message contents
     * @return int - >0 (number of sent B) if successful, -1 if failed
     */
    int message(const std::string &display_name, const std::string &message_contents);

    /**
     * @brief Send the ERR message to the server.
     *
     * @param display_name - the display name
     * @param message_contents - the message contents
     * @return int - >0 (number of sent B) if successful, -1 if failed
     */
    int error(const std::string &display_name, const std::string &message_contents);

    /**
     * @brief Send the BYE message to the server.
     *
     * @return int - >0 (number of sent B) if successful, -1 if failed
     */
    int bye();

    /**
     * @brief Get the client socket file descriptor.
     *
     * @return int - the client socket file descriptor
     */
    int get_client_socket();

    /**
     * @brief Get the server address.
     *
     * @return struct sockaddr_in - the server address
     */
    struct sockaddr_in get_server_address();

    /**
     * @brief Wait for a message from the server.
     *
     * @return Message - the message
     */
    Message receive();

    /**
     * @brief Wait for a CONFIRM message from the server with a timeout, all other messages are pushed to a message stack.
     *
     * @param timeout_ms
     * @return Message
     */
    Message receive_confirm(int timeout_ms);

    /**
     * @brief Wait for a message from the server with a timeout and retry sending las message.
     *
     * @param timeout_s - the timeout in seconds
     * @param max_retries - the maximum number of retries
     * @return Message - the message
     */
    Message receive_with_retry(int timeout_s, int max_retries);

    /**
     * @brief Get the boolean value of the reply message.
     *
     * @param msg
     * @return true
     * @return false
     */
    static bool get_reply(Message msg);
};

#endif // POSTMAN_H