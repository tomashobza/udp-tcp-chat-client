// #include "Postman.hpp"

// class TCPPostman : public IPostman
// {
// public:
//     TCPPostman(Args args);
//     ~TCPPostman();

//     void attach_to_server(const std::string &server_hostname, uint16_t port_number) override;
//     int authorize(const std::string &username, const std::string &display_name, const std::string &password) override;
//     int join(const std::string &channel_id, const std::string &display_name) override;
//     int message(const std::string &display_name, const std::string &message_contents) override;
//     int error(const std::string &display_name, const std::string &message_contents) override;
//     int bye() override;
//     int confirm() override;
//     int get_client_socket() override;
//     struct sockaddr_in get_server_address() override;
//     PollResults poll_for_messages() override;

//     Message receive() override;
// };