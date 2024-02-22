#include "postman.h"

Postman::Postman() : client_socket()
{
    std::cout << "postman created" << std::endl;
}

Postman::~Postman()
{
    // Automatic cleanup happens here, no need for explicit calls
}

void Postman::attach_to_server(const std::string &server_hostname, uint16_t port_number)
{
    std::memset(&server_address, 0, sizeof(server_address));

    struct hostent *server = gethostbyname(server_hostname.c_str());
    if (server == nullptr)
    {
        throw std::runtime_error("ERROR, no such host as '" + server_hostname + "'");
    }

    server_address.sin_family = AF_INET;
    std::memcpy(&server_address.sin_addr.s_addr, server->h_addr, server->h_length);
    server_address.sin_port = htons(port_number);
}

int Postman::test_send()
{
    std::string message = "Hello, world!";
    ssize_t n = sendto(client_socket.getFd(), message.c_str(), message.length(), 0,
                       (struct sockaddr *)&server_address, sizeof(server_address));
    if (n < 0)
    {
        throw std::runtime_error("ERROR sending test message");
    }
    return 0;
}

int Postman::authorize(const std::string &username, const std::string &display_name, const std::string &password)
{
    // get the message data length
    size_t data_len = BEG_OFFSET + username.length() + STR_OFFSET + display_name.length() + STR_OFFSET + password.length() + STR_OFFSET;

    // create the message data buffer
    std::vector<uint8_t> data(data_len);
    data[0] = MessageType::AUTH;
    std::memcpy(&data[1], &msg_id, sizeof(MessageID));
    std::memcpy(&data[BEG_OFFSET], username.c_str(), username.length());
    std::memcpy(&data[BEG_OFFSET + username.length() + STR_OFFSET], display_name.c_str(), display_name.length());
    std::memcpy(&data[BEG_OFFSET + username.length() + STR_OFFSET + display_name.length() + STR_OFFSET], password.c_str(), password.length());

    // send the message
    ssize_t n = sendto(client_socket.getFd(), data.data(), data_len, 0,
                       (struct sockaddr *)&server_address, sizeof(server_address));
    // increment the message ID
    msg_id++;

    // check for errors
    if (n < 0)
    {
        throw std::runtime_error("ERROR sending AUTH message");
    }
    return 0;
}

int Postman::join(const std::string &channel_id, const std::string &display_name)
{
    // get the message data length
    size_t data_len = BEG_OFFSET + channel_id.length() + STR_OFFSET + display_name.length();

    // create the message data buffer
    std::vector<uint8_t> data(data_len);
    data[0] = MessageType::JOIN;
    std::memcpy(&data[1], &msg_id, sizeof(MessageID));
    std::memcpy(&data[BEG_OFFSET], channel_id.c_str(), channel_id.length());
    std::memcpy(&data[BEG_OFFSET + channel_id.length() + STR_OFFSET], display_name.c_str(), display_name.length());

    // send the message
    ssize_t n = sendto(client_socket.getFd(), data.data(), data_len, 0,
                       (struct sockaddr *)&server_address, sizeof(server_address));
    // increment the message ID
    msg_id++;

    // check for errors
    if (n < 0)
    {
        throw std::runtime_error("ERROR sending JOIN message");
    }
    return 0;
}

int Postman::message(const std::string &display_name, const std::string &message_contents)
{
    // get the message data length
    size_t data_len = BEG_OFFSET + display_name.length() + STR_OFFSET + message_contents.length();

    // create the message data buffer
    std::vector<uint8_t> data(data_len);
    data[0] = MessageType::MSG;
    std::memcpy(&data[1], &msg_id, sizeof(MessageID));
    std::memcpy(&data[BEG_OFFSET], display_name.c_str(), display_name.length());
    std::memcpy(&data[BEG_OFFSET + display_name.length() + STR_OFFSET], message_contents.c_str(), message_contents.length());

    // send the message
    ssize_t n = sendto(client_socket.getFd(), data.data(), data_len, 0,
                       (struct sockaddr *)&server_address, sizeof(server_address));
    // increment the message ID
    msg_id++;

    // check for errors
    if (n < 0)
    {
        throw std::runtime_error("ERROR sending MSG message");
    }
    return 0;
}

int Postman::error(const std::string &display_name, const std::string &message_contents)
{
    // get the message data length
    size_t data_len = BEG_OFFSET + display_name.length() + STR_OFFSET + message_contents.length();

    // create the message data buffer
    std::vector<uint8_t> data(data_len);
    data[0] = MessageType::ERR;
    std::memcpy(&data[1], &msg_id, sizeof(MessageID));
    std::memcpy(&data[BEG_OFFSET], display_name.c_str(), display_name.length());
    std::memcpy(&data[BEG_OFFSET + display_name.length() + STR_OFFSET], message_contents.c_str(), message_contents.length());

    // send the message
    ssize_t n = sendto(client_socket.getFd(), data.data(), data_len, 0,
                       (struct sockaddr *)&server_address, sizeof(server_address));
    // increment the message ID
    msg_id++;

    // check for errors
    if (n < 0)
    {
        throw std::runtime_error("ERROR sending MSG message");
    }
    return 0;
}

int Postman::bye()
{
    // get the message data length
    size_t data_len = BEG_OFFSET;

    // create the message data buffer
    std::vector<uint8_t> data(data_len);
    data[0] = MessageType::BYE;
    std::memcpy(&data[1], &msg_id, sizeof(MessageID));

    // send the message
    ssize_t n = sendto(client_socket.getFd(), data.data(), data_len, 0,
                       (struct sockaddr *)&server_address, sizeof(server_address));
    // increment the message ID
    msg_id++;

    // check for errors
    if (n < 0)
    {
        throw std::runtime_error("ERROR sending BYE message");
    }
    return 0;
}

int Postman::get_client_socket()
{
    return client_socket.getFd();
}

struct sockaddr_in Postman::get_server_address()
{
    return server_address;
}

std::vector<uint8_t> Postman::receive()
{
    // Allocate a buffer with a maximum expected size
    size_t maxBufferSize = 1024; // Adjust this size according to your needs
    std::vector<uint8_t> *buffer = new std::vector<uint8_t>(maxBufferSize);

    // receive the message
    ssize_t n = recvfrom(client_socket.getFd(), buffer->data(), buffer->size(), 0, NULL, NULL);
    if (n < 0)
    {
        // Handle error or return nullptr to indicate an error condition
        delete buffer; // Important to avoid memory leaks
        throw std::runtime_error("ERROR receiving message");
    }

    // Resize the buffer to the actual received size to avoid excess memory usage
    buffer->resize(n);

    return *buffer;
}