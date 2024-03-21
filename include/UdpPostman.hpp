#include "Postman.hpp"

class UDPPostman : public IPostman
{
private:
    /** List of messages waiting for confirmation */
    std::list<ConfirmWaiter> confirm_waiters;
    /** Time of the last confirm_waiters check */
    long long timestamp;
    /** First message flag */
    bool first_message = true;

public:
    /**
     * @brief Handle the SIGINT signal.
     *
     * @param signal - the signal number
     */
    static void handle_sigint(int signal);

    UDPPostman(Args args);
    ~UDPPostman();

    void attach_to_server(const std::string &server_hostname, uint16_t port_number) override;
    int authorize(const std::string &username, const std::string &display_name, const std::string &password) override;
    int join(const std::string &channel_id, const std::string &display_name) override;
    int message(const std::string &display_name, const std::string &message_contents) override;
    int error(const std::string &display_name, const std::string &message_contents) override;
    int bye() override;
    int confirm() override;
    int confirm(MessageID ref_id);
    int get_client_socket() override;
    struct sockaddr_in get_server_address() override;
    PollResults poll_for_messages() override;

    Message receive() override;

    void allow_client_commands(std::vector<CommandType> messages) override;

    PollResults handle_server_message() override;
    PollResults handle_user_command() override;

    Message data_to_message(std::vector<uint8_t> data);

    bool check_waiters();
};