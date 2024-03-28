#include "Postman.hpp"

class UDPPostman : public IPostman
{
private:
    /** The ID of the next message to be sent. */
    uint16_t msg_id = 0;
    /** The ID of the last message received. */
    int16_t ref_msg_id = 0;
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
    static void udp_handle_sigint(int signal);

    UDPPostman(Args args);
    ~UDPPostman();

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

    /**
     * @brief Send the CONFIRM message to the server.
     *
     * @return int - >0 (number of sent B) if successful, -1 if failed
     */
    int confirm();

    /**
     * @brief Send the CONFIRM message to the server.
     *
     * @param ref_id - ReferenceID of the CONFIRM message
     * @return int - >0 (number of sent B) if successful, -1 if failed
     */
    int confirm(MessageID ref_id);

    bool check_waiters();
};