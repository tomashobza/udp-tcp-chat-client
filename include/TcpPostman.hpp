#include "Postman.hpp"
#include <regex>

class TCPPostman : public IPostman
{
public:
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