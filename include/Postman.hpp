#ifndef POSTMAN_H
#define POSTMAN_H

#include <iostream>
#include <cstring>
#include <netdb.h>
#include <csignal>
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
#include "Utils.hpp"

extern bool tcp_had_sigint;

class IPostman
{
protected:
    /** Client side socket file descriptor */
    int client_socket;
    /** Server address */
    struct sockaddr_in server_address;
    /** The timestamp of the last time check. */
    long long timestamp = std::chrono::system_clock::now().time_since_epoch().count();
    /** Display name of the user */
    std::string display_name = "User";
    /** List of allowed server messages */
    std::vector<MessageType> allowed_server_messages = {};
    /** List of allowed client commands */
    std::vector<CommandType> allowed_client_commands = {CommandType::CMD_HELP};
    /** Last sent message */
    Message last_sent_message;
    /** Is user input allowed flag */
    bool is_waiting_for_reply = false;

public:
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
     * @brief Get the display name of the user
     *
     * @return std::string - display name
     */
    virtual std::string get_display_name() = 0;

    /**
     * @brief Attach to a server by hostname and port number.
     *
     * @param server_hostname - the hostname of the server
     * @param port_number - the port number of the server
     */
    virtual void attach_to_server(const std::string &server_hostname, uint16_t port_number) = 0;

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
     * @brief Receive a message from the server or stdin.
     *
     * @return Message
     */
    virtual PollResults poll_for_messages() = 0;

    virtual Message receive() = 0;

    // TODO: add comments
    virtual void allow_client_commands(std::vector<CommandType> messages) = 0;

    virtual PollResults handle_server_message() = 0;
    virtual PollResults handle_user_command() = 0;

    virtual Message data_to_message(std::vector<uint8_t> data) = 0;
};

#endif // POSTMAN_H