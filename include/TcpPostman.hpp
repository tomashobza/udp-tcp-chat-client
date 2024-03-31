/**
 * @file TcpPostman.hpp
 * @author Tomáš Hobza (xhobza03)
 * @brief TCP Postman class for the project
 * @date 2024-03-31
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "Postman.hpp"
#include <regex>

/**
 * @brief TCP Postman class
 *
 */
class TCPPostman : public IPostman
{
public:
    /**
     * @brief Handle the SIGINT signal.
     *
     * @param signal - the signal number
     */
    static void tcp_handle_sigint(int signal);

    TCPPostman(Args args);
    ~TCPPostman();

    std::string get_display_name() override;
    void attach_to_server(const std::string &server_hostname, uint16_t port_number) override;
    int authorize(const std::string &username, const std::string &display_name, const std::string &password) override;
    int join(const std::string &channel_id, const std::string &display_name) override;
    int message(const std::string &display_name, const std::string &message_contents) override;
    int error(const std::string &display_name, const std::string &message_contents) override;
    int bye() override;
    PollResults poll_for_messages() override;
    Message receive() override;
    void allow_client_commands(std::vector<CommandType> messages) override;
    PollResults handle_server_message() override;
    PollResults handle_user_command() override;
    Message data_to_message(std::vector<uint8_t> data) override;
};