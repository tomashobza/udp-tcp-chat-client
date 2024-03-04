#include "postman.h"

void UDPPostman::attach_to_server(const std::string &server_hostname, uint16_t port_number)
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

int UDPPostman::authorize(const std::string &username, const std::string &display_name, const std::string &password)
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

    // check for errors
    if (n < 0)
    {
        throw std::runtime_error("ERROR sending AUTH message");
    }

    // Push the message to the queue of messages to be confirmed
    confirm_waiters.push_back(ConfirmWaiter{MSG_MAX_RETRIES, MSG_TIMEOUT, (MessageID)msg_id, data});

    // increment the message ID
    msg_id++;

    return 0;
}

int UDPPostman::join(const std::string &channel_id, const std::string &display_name)
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

int UDPPostman::message(const std::string &display_name, const std::string &message_contents)
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

int UDPPostman::error(const std::string &display_name, const std::string &message_contents)
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

int UDPPostman::bye()
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

int UDPPostman::confirm()
{
    std::clog << "> CONFIRM " << ref_msg_id << std::endl;

    // get the message data length
    size_t data_len = BEG_OFFSET;

    // create the message data buffer
    std::vector<uint8_t> data(data_len);
    data[0] = MessageType::CONFIRM;
    std::memcpy(&data[1], &ref_msg_id, sizeof(MessageID));

    // send the message
    ssize_t n = sendto(client_socket.getFd(), data.data(), data_len, 0,
                       (struct sockaddr *)&server_address, sizeof(server_address));

    // check for errors
    if (n < 0)
    {
        throw std::runtime_error("ERROR sending BYE message");
    }

    return 0;
}

int UDPPostman::get_client_socket()
{
    return client_socket.getFd();
}

struct sockaddr_in UDPPostman::get_server_address()
{
    return server_address;
}

Message UDPPostman::receive()
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
    msg.id = ntohs(msg.id);

    if (msg.type == MessageType::CONFIRM)
    {

        // If the message is a confirmation, remove the message from the queue
        if (!confirm_waiters.empty())
        {
            for (auto it = confirm_waiters.begin(); it != confirm_waiters.end(); it++)
            {

                if (it->id == msg.id)
                {
                    confirm_waiters.erase(it);
                    std::clog << "CONFIRMED " << msg.id << std::endl;
                    break;
                }
            }
        }
    }
    else
    {
        ref_msg_id = msg.id;

        // TODO: TOMASI TY DEBILE TOTO SMAZAT
        std::cout << "< " << (int)msg.type << ":" << (int)msg.id << ":'";
        for (int i = 0; i < (int)msg.data.size(); i++)
        {
            std::cout << (char)msg.data.at(i);
        }
        std::cout << "'" << std::endl;
    }

    return msg;
}

void UDPPostman::check_waiters()
{
    if (confirm_waiters.empty())
    {
        return;
    }

    int elapsed = this->check_time();

    // Check if any of the confirm_waiters have timed out
    for (auto it = confirm_waiters.begin(); it != confirm_waiters.end(); it++)
    {
        // If the waiter has timed out, resend the message
        if (it->time_left < elapsed)
        {
            // If the waiter has tries left, resend the message
            if (it->tries_left > 0)
            {
                std::clog << "resending message: " << (int)it->id << std::endl;

                // Send the message
                ssize_t n = sendto(this->get_client_socket(), it->data.data(), it->data.size(), 0, (struct sockaddr *)&server_address, sizeof(server_address));
                // Check for errors
                if (n < 0)
                {
                    throw std::runtime_error("ERROR sending message");
                }

                // Reset the timer and decrement the number of tries
                it->time_left = MSG_TIMEOUT;
                it->tries_left--;
            }
            else
            {
                // If the waiter has no more tries left, remove it from the queue
                confirm_waiters.erase(it);
                std::cerr << "ERROR: message not confirmed" << std::endl;
            }
        }

        it->time_left -= elapsed;
    }
}

MessageID UDPPostman::get_msg_id()
{
    return msg_id;
}

void UDPPostman::start_timer()
{
    start_time = std::chrono::system_clock::now();
}

int UDPPostman::check_time()
{
    auto end_time = std::chrono::system_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    return elapsed_time;
}