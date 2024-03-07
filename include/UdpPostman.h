#include "Postman.h"

class UDPPostman : public IPostman
{
private:
    std::list<ConfirmWaiter> confirm_waiters;

public:
    UDPPostman(Args args);
    ~UDPPostman();

    void attach_to_server(const std::string &server_hostname, uint16_t port_number) override;
    int authorize(const std::string &username, const std::string &display_name, const std::string &password) override;
    int join(const std::string &channel_id, const std::string &display_name) override;
    int message(const std::string &display_name, const std::string &message_contents) override;
    int error(const std::string &display_name, const std::string &message_contents) override;
    int bye() override;
    int confirm() override;
    int get_client_socket() override;
    struct sockaddr_in get_server_address() override;
    PollResult poll_for_messages() override;

    ConfirmMessage data_to_confirm(std::vector<uint8_t> data) override;
    ReplyMessage data_to_reply(std::vector<uint8_t> data) override;
    AuthMessage data_to_auth(std::vector<uint8_t> data) override;
    JoinMessage data_to_join(std::vector<uint8_t> data) override;
    MsgMessage data_to_msg(std::vector<uint8_t> data) override;
    ErrMessage data_to_err(std::vector<uint8_t> data) override;
    ByeMessage data_to_bye(std::vector<uint8_t> data) override;
};