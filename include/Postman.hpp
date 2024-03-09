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
#include <chrono>
#include <list>
#include <thread>
#include <poll.h>
#include "types.hpp"
#include "constants.hpp"
#include "InputParser.hpp"

class IPostman
{
protected:
    /** Client side socket file descriptor */
    int client_socket;
    /** Server address */
    struct sockaddr_in server_address;
    /** The ID of the next message to be sent. */
    uint16_t msg_id = 0;
    /** The ID of the last message received. */
    uint16_t ref_msg_id = 0;
    /** The timestamp of the last time check. */
    std::chrono::time_point<std::chrono::system_clock> timestamp;
    /** Display name */
    std::string display_name;

public:
    /**
     * @brief Attach to a server by hostname and port number.
     *
     * @param server_hostname - the hostname of the server
     * @param port_number - the port number of the server
     */
    virtual void attach_to_server(const std::string &server_hostname, uint16_t port_number) = 0;

    /**
     * @brief Send the CONFIRM message to the server.
     *
     * @return int - >0 (number of sent B) if successful, -1 if failed
     */
    virtual int confirm() = 0;

    /**
     * @brief Send the AUTH message to the server.
     *
     * @param username - the username
     * @param display_name - the display name
     * @param password - the password
     * @return int - >0 (number of sent B) if successful, -1 if failed
     */
    virtual int authorize(const std::string &username, const std::string &display_name, const std::string &password) = 0;

    /**
     * @brief Send the JOIN message to the server.
     *
     * @param channel_id - the channel ID
     * @param DisplayName - the display name
     * @return int - >0 (number of sent B) if successful, -1 if failed
     */
    virtual int join(const std::string &channel_id, const std::string &display_name) = 0;

    /**
     * @brief Send the MSG message to the server.
     *
     * @param display_name - the display name
     * @param message_contents - the message contents
     * @return int - >0 (number of sent B) if successful, -1 if failed
     */
    virtual int message(const std::string &display_name, const std::string &message_contents) = 0;

    /**
     * @brief Send the ERR message to the server.
     *
     * @param display_name - the display name
     * @param message_contents - the message contents
     * @return int - >0 (number of sent B) if successful, -1 if failed
     */
    virtual int error(const std::string &display_name, const std::string &message_contents) = 0;

    /**
     * @brief Send the BYE message to the server.
     *
     * @return int - >0 (number of sent B) if successful, -1 if failed
     */
    virtual int bye() = 0;

    /**
     * @brief Get the client socket file descriptor.
     *
     * @return int - the client socket file descriptor
     */
    virtual int get_client_socket() = 0;

    /**
     * @brief Get the server address.
     *
     * @return struct sockaddr_in - the server address
     */
    virtual struct sockaddr_in get_server_address() = 0;

    /**
     * @brief Receive a message from the server or stdin.
     *
     * @return Message
     */
    virtual PollResults poll_for_messages() = 0;

    virtual Message receive() = 0;

    // TODO: add comments
    virtual Message data_to_message(std::vector<uint8_t> data) = 0;
};

#endif // POSTMAN_H