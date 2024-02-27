#include "postman.h"

Postman::Postman() : client_socket()
{
    // // Set the socket to non-blocking mode
    // client_socket.setSocketNonBlocking();
    std::clog << "Created client socket: " << client_socket.getFd() << std::endl;
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

    last_message = data;
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

    last_message = data;
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

    last_message = data;
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

    last_message = data;
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

    last_message = data;
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

Message Postman::receive()
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
        return Message{
            MessageType::ERR,
            0,
            std::vector<uint8_t>(0)};
    }

    // Resize the buffer to the actual received size to avoid excess memory usage
    buffer->resize(n);

    // Create a message object
    Message msg;
    msg.data = *buffer;
    msg.type = (MessageType)buffer->at(0);
    std::memcpy(&msg.id, &buffer->at(1), sizeof(MessageID));

    return msg;
}

Message Postman::receive_confirm(int timeout_ms)
{
    // Set the timeout for the socket
    client_socket.set_timeout(timeout_ms);
    Message msg = {
        MessageType::ERR,
        0,
        std::vector<uint8_t>(0)}; // return buffer

    int remaining_time_ms = 250;

    while (remaining_time_ms > 0)
    {
        // Start the timer
        auto start_time = std::chrono::high_resolution_clock::now();

        // Try to receive the message
        msg = receive();

        // Stop the timer
        auto elapsed = std::chrono::high_resolution_clock::now() - start_time;

        // Subtract the elapsed time from the remaining time
        remaining_time_ms -= std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();

        // If the remaining time is less than or equal to 0, break the loop
        if (remaining_time_ms <= 0)
        {
            break;
        }

        // If the buffer is not empty and the message is a confirm, return it
        if (msg.data.size() > 0 && msg.id == CONFIRM)
        {
            // Unset the timeout for the socket
            client_socket.unset_timeout();
            return msg;
        }
        else if (msg.data.size() > 0 && msg.id != CONFIRM)
        {
            // If the buffer is not empty and the message is not a confirm, push it to the stack
            message_stack.push(msg);
        }
    }

    // Unset the timeout for the socket
    client_socket.unset_timeout();
    return msg;
}

Message Postman::receive_with_retry(int timeout_ms, int max_retries)
{
    // Set the timeout for the socket
    client_socket.set_timeout(timeout_ms);
    Message msg; // return buffer

    // Try to receive the message max_retries times
    for (int i = 0; i < max_retries; i++)
    {
        // Try to receive the message
        msg = receive();

        // If the buffer is not empty, return it
        if (msg.data.size() > 0)
        {
            // Unset the timeout for the socket
            client_socket.unset_timeout();
            return msg;
        }

        std::clog << "RETRY" << std::endl;

        // If the buffer is empty, send the last message again
        ssize_t n = sendto(client_socket.getFd(), last_message.data(), last_message.size(), 0, (struct sockaddr *)&server_address, sizeof(server_address));
        // If the message was sent successfully, continue to the next iteration
        if (n < 0)
        {
            // Else unset the timeout for the socket and throw an error
            client_socket.unset_timeout();
            throw std::runtime_error("ERROR sending last message with retry");
            return msg;
        }
        continue;
    }

    // Unset the timeout for the socket
    client_socket.unset_timeout();

    // TODO: maybe handle this error better
    throw std::runtime_error("ERROR confirm not recieved");
    return msg;
}