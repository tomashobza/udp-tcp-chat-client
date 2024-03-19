#include "UdpPostman.hpp"

/** Had SIGINT flag */
bool had_sigint = false;

void UDPPostman::handle_sigint(int signal)
{
    if (signal == SIGINT)
    {
        had_sigint = true;
    }
}

UDPPostman::UDPPostman(Args args)
{
    std::signal(SIGINT, UDPPostman::handle_sigint);

    // Create the client socket
    client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_socket < 0)
    {
        throw std::runtime_error("ERROR opening socket");
    }

    // Attach to the server
    this->attach_to_server(args.hostname, args.port);
}

UDPPostman::~UDPPostman()
{
    close(client_socket);
}

void UDPPostman::attach_to_server(const std::string &server_hostname, uint16_t port_number)
{
    // Set the server address
    std::memset(&server_address, 0, sizeof(server_address));

    // Get the server by its hostname
    struct hostent *server = gethostbyname(server_hostname.c_str());
    if (server == nullptr)
    {
        throw std::runtime_error("ERROR, no such host as '" + server_hostname + "'");
    }

    // Set the server address
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
    data[1] = (uint8_t)msg_id >> 8;
    data[2] = (uint8_t)msg_id & 0xFF;
    std::memcpy(&data[BEG_OFFSET], username.c_str(), username.length());
    std::memcpy(&data[BEG_OFFSET + username.length() + STR_OFFSET], display_name.c_str(), display_name.length());
    std::memcpy(&data[BEG_OFFSET + username.length() + STR_OFFSET + display_name.length() + STR_OFFSET], password.c_str(), password.length());

    // send the message
    ssize_t n = sendto(client_socket, data.data(), data_len, 0,
                       (struct sockaddr *)&server_address, sizeof(server_address));

    // check for errors
    if (n < 0)
    {
        throw std::runtime_error("ERROR sending AUTH message");
    }

    // Push the message to the queue of messages to be confirmed
    confirm_waiters.push_back(ConfirmWaiter{MSG_MAX_RETRIES, Utils::get_timestamp() + MSG_TIMEOUT, (MessageID)msg_id, data});

    last_sent_message = Message{};
    last_sent_message.type = MessageType::AUTH;
    last_sent_message.id = msg_id;
    last_sent_message.username = username;
    last_sent_message.display_name = display_name;
    last_sent_message.password = password;

    // increment the message ID
    msg_id++;

    // Disable user input after joining a channel until a reply is received
    is_waiting_for_reply = true;

    return 0;
}

int UDPPostman::join(const std::string &channel_id, const std::string &display_name)
{
    // get the message data length
    size_t data_len = BEG_OFFSET + channel_id.length() + STR_OFFSET + display_name.length();

    // create the message data buffer
    std::vector<uint8_t> data(data_len);
    data[0] = MessageType::JOIN;
    data[1] = (uint8_t)msg_id >> 8;
    data[2] = (uint8_t)msg_id & 0xFF;
    std::memcpy(&data[BEG_OFFSET], channel_id.c_str(), channel_id.length());
    std::memcpy(&data[BEG_OFFSET + channel_id.length() + STR_OFFSET], display_name.c_str(), display_name.length());

    // send the message
    ssize_t n = sendto(client_socket, data.data(), data_len, 0,
                       (struct sockaddr *)&server_address, sizeof(server_address));

    // check for errors
    if (n < 0)
    {
        throw std::runtime_error("ERROR sending JOIN message");
    }

    // Push the message to the queue of messages to be confirmed
    confirm_waiters.push_back(ConfirmWaiter{MSG_MAX_RETRIES, Utils::get_timestamp() + MSG_TIMEOUT, (MessageID)msg_id, data});

    last_sent_message = Message{};
    last_sent_message.type = MessageType::JOIN;
    last_sent_message.id = msg_id;
    last_sent_message.channel_id = channel_id;
    last_sent_message.display_name = display_name;

    // increment the message ID
    msg_id++;

    // Disable user input after joining a channel until a reply is received
    is_waiting_for_reply = true;

    return 0;
}

int UDPPostman::message(const std::string &display_name, const std::string &message_contents)
{
    // get the message data length
    size_t data_len = BEG_OFFSET + display_name.length() + STR_OFFSET + message_contents.length();

    // create the message data buffer
    std::vector<uint8_t> data(data_len);
    data[0] = MessageType::MSG;
    data[1] = (uint8_t)msg_id >> 8;
    data[2] = (uint8_t)msg_id & 0xFF;
    std::memcpy(&data[BEG_OFFSET], display_name.c_str(), display_name.length());
    std::memcpy(&data[BEG_OFFSET + display_name.length() + STR_OFFSET], message_contents.c_str(), message_contents.length());

    std::clog << "vyhul: " << (int)data[1] << std::endl;
    std::clog << "mipenis: " << (int)data[2] << std::endl;

    // send the message
    ssize_t n = sendto(client_socket, data.data(), data_len, 0,
                       (struct sockaddr *)&server_address, sizeof(server_address));

    // check for errors
    if (n < 0)
    {
        throw std::runtime_error("ERROR sending MSG message");
    }

    std::cout << "Message sent!" << msg_id - 1 << std::endl;

    // Push the message to the queue of messages to be confirmed
    confirm_waiters.push_back(ConfirmWaiter{MSG_MAX_RETRIES, Utils::get_timestamp() + MSG_TIMEOUT, (MessageID)msg_id, data});

    last_sent_message = Message{};
    last_sent_message.type = MessageType::MSG;
    last_sent_message.id = msg_id;
    last_sent_message.display_name = display_name;
    last_sent_message.contents = message_contents;

    // increment the message ID
    msg_id++;

    return 0;
}

int UDPPostman::error(const std::string &display_name, const std::string &message_contents)
{
    // get the message data length
    size_t data_len = BEG_OFFSET + display_name.length() + STR_OFFSET + message_contents.length();

    // create the message data buffer
    std::vector<uint8_t> data(data_len);
    data[0] = MessageType::ERR;
    data[1] = (uint8_t)msg_id >> 8;
    data[2] = (uint8_t)msg_id & 0xFF;
    std::memcpy(&data[BEG_OFFSET], display_name.c_str(), display_name.length());
    std::memcpy(&data[BEG_OFFSET + display_name.length() + STR_OFFSET], message_contents.c_str(), message_contents.length());

    // send the message
    ssize_t n = sendto(client_socket, data.data(), data_len, 0,
                       (struct sockaddr *)&server_address, sizeof(server_address));

    // check for errors
    if (n < 0)
    {
        throw std::runtime_error("ERROR sending MSG message");
    }

    // Push the message to the queue of messages to be confirmed
    confirm_waiters.push_back(ConfirmWaiter{MSG_MAX_RETRIES, Utils::get_timestamp() + MSG_TIMEOUT, (MessageID)msg_id, data});

    last_sent_message = Message{};
    last_sent_message.type = MessageType::ERR;
    last_sent_message.id = msg_id;
    last_sent_message.display_name = display_name;
    last_sent_message.contents = message_contents;

    // increment the message ID
    msg_id++;

    return 0;
}

int UDPPostman::bye()
{
    // get the message data length
    size_t data_len = BEG_OFFSET;

    // create the message data buffer
    std::vector<uint8_t> data(data_len);
    data[0] = MessageType::BYE;
    data[1] = (uint8_t)msg_id >> 8;
    data[2] = (uint8_t)msg_id & 0xFF;

    // send the message
    ssize_t n = sendto(client_socket, data.data(), data_len, 0,
                       (struct sockaddr *)&server_address, sizeof(server_address));

    // check for errors
    if (n < 0)
    {
        throw std::runtime_error("ERROR sending BYE message");
    }

    // Push the message to the queue of messages to be confirmed
    confirm_waiters.push_back(ConfirmWaiter{MSG_MAX_RETRIES, Utils::get_timestamp() + MSG_TIMEOUT, (MessageID)msg_id, data});

    last_sent_message = Message{};
    last_sent_message.type = MessageType::BYE;
    last_sent_message.id = msg_id;

    // increment the message ID
    msg_id++;

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
    ssize_t n = sendto(client_socket, data.data(), data_len, 0,
                       (struct sockaddr *)&server_address, sizeof(server_address));

    // check for errors
    if (n < 0)
    {
        throw std::runtime_error("ERROR sending BYE message");
    }

    // Push the message to the queue of messages to be confirmed
    confirm_waiters.push_back(ConfirmWaiter{MSG_MAX_RETRIES, Utils::get_timestamp() + MSG_TIMEOUT, (MessageID)msg_id, data});

    last_sent_message = Message{};
    last_sent_message.type = MessageType::CONFIRM;
    last_sent_message.ref_id = ref_msg_id;

    // increment the message ID
    msg_id++;

    return 0;
}

int UDPPostman::get_client_socket()
{
    return client_socket;
}

struct sockaddr_in UDPPostman::get_server_address()
{
    return server_address;
}

PollResults UDPPostman::poll_for_messages()
{
    PollResults results;

    // Check if stdin is closed
    if (Utils::is_stdin_closed() || had_sigint)
    {
        // Send the BYE message
        PollResult res;
        res.type = PollResultType::USER;
        res.message.type = MessageType::BYE;
        res.message.id = msg_id;
        msg_id++;
        results.push_back(res);
        return results;
    }

    // Check if there are any messages to be confirmed
    this->check_waiters();

    // Set up poll to listen on stdin and the socket
    struct pollfd fds[2];
    // Listen to stdin
    fds[0].fd = STDIN_FILENO; // stdin
    fds[0].events = POLLIN;
    // Listen to socket
    fds[1].fd = client_socket;
    fds[1].events = POLLIN;

    // Poll for messages on the socket and stdin
    int ret = poll(fds, 2, 50);
    if (ret == -1)
    {
        // An error occurred, check if it was due to a SIGINT signal
        if (had_sigint)
        {
            PollResult res;
            res.type = PollResultType::USER;
            res.message.type = MessageType::BYE;
            res.message.id = msg_id;
            msg_id++;
            results.push_back(res);
            return results;
        }
        else
        {
            throw std::runtime_error("ERROR polling for messages");
        }
    }

    // Check if there are any messages to be confirmed
    this->check_waiters();

    bool no_confirm_waiters = confirm_waiters.empty();

    // If stdin has data and there are no messages to be confirmed and the client is not waiting for a reply
    if (fds[0].revents & POLLIN && no_confirm_waiters && !is_waiting_for_reply)
    {
        // Parse the input
        Command cmd = InputParser::parse_input();

        bool is_allowed = std::find(allowed_client_commands.begin(), allowed_client_commands.end(), cmd.type) != allowed_client_commands.end();

        if (!is_allowed)
        {
            std::cerr << "ERR: Command not allowed in this state!" << std::endl;
        }
        else
        {
            // Execute the command or save the message to be sent
            switch (cmd.type)
            {
            case CommandType::CMD_AUTH:
            {
                Message auth_msg;
                auth_msg.type = MessageType::AUTH;
                auth_msg.id = msg_id;
                auth_msg.username = cmd.args[0];
                auth_msg.password = cmd.args[1];
                auth_msg.display_name = cmd.args[2];
                results.push_back(PollResult{
                    PollResultType::USER,
                    auth_msg});

                display_name = cmd.args[2];

                break;
            }
            case CommandType::CMD_JOIN:
            {
                Message join_msg;
                join_msg.type = MessageType::JOIN;
                join_msg.id = msg_id;
                join_msg.display_name = display_name;
                join_msg.channel_id = cmd.args[0];

                results.push_back(PollResult{
                    PollResultType::USER,
                    join_msg});
                break;
            }
            case CommandType::CMD_RENAME:
                display_name = cmd.args[0];
                break;
            case CommandType::CMD_MSG:
            {
                Message msg_msg;
                msg_msg.type = MessageType::MSG;
                msg_msg.id = msg_id;
                msg_msg.display_name = display_name;
                msg_msg.contents = cmd.args[0];
                results.push_back(PollResult{
                    PollResultType::USER,
                    msg_msg});
                break;
            }
            default:
                break;
            }
        }
    }

    // If socket has data
    if (fds[1].revents & POLLIN)
    {
        // Receive the message
        Message msg = this->receive();

        bool is_contentful = msg.type != MessageType::UNKNOWN && msg.type != MessageType::CONFIRM;
        bool is_first = first_message;
        bool is_new = (msg.id > ref_msg_id || is_first);

        // TODO: handle incorrect reply ref msg id

        // Add the message to the results if it is not a confirmation or an unknown message
        if (is_contentful && is_new)
        {
            results.push_back(PollResult{
                PollResultType::SERVER,
                msg});

            ref_msg_id = msg.id;
            first_message = false;
        }
    }

    return results;
}

Message UDPPostman::receive()
{
    // Allocate a buffer with a maximum expected size
    size_t maxBufferSize = 1024; // Adjust this size according to your needs
    std::vector<uint8_t> *buffer = new std::vector<uint8_t>(maxBufferSize);

    sockaddr new_server_address;
    socklen_t new_server_address_len = sizeof(server_address);

    // receive the message
    ssize_t n = recvfrom(client_socket, buffer->data(), buffer->size(), 0, &new_server_address, &new_server_address_len);
    if (n < 0)
    {
        // Handle error or return nullptr to indicate an error condition
        delete buffer; // Important to avoid memory leaks
        Message unknown;
        unknown.type = MessageType::UNKNOWN;
        return unknown;
    }

    // Resize the buffer to the actual received size to avoid excess memory usage
    buffer->resize(n);

    // Create a message object
    Message msg = data_to_message(*buffer);

    // If the message is a confirmation, remove the waiting message from the queue
    if (msg.type == MessageType::CONFIRM)
    {
        // If the message is a confirmation, remove the message from the queue
        if (!confirm_waiters.empty())
        {
            for (auto it = confirm_waiters.begin(); it != confirm_waiters.end(); it++)
            {
                // Check if the message is the one being waited for
                if (it->id == msg.ref_id)
                {
                    if ((MessageType)(it->data.at(0)) == MessageType::AUTH)
                    {
                        sockaddr_in *sender_addr = reinterpret_cast<sockaddr_in *>(&new_server_address);
                        this->server_address.sin_port = sender_addr->sin_port;
                    }
                    confirm_waiters.erase(it);
                    std::clog << FGRN("CONFIRMED ") << msg.ref_id << std::endl;
                    break;
                }
            }
        }
    }
    else if (msg.type == MessageType::REPLY)
    {
        // If the message is a reply, update the waiting flag
        is_waiting_for_reply = false;
    }

    return msg;
}

void UDPPostman::allow_client_commands(std::vector<CommandType> messages)
{
    // Help is always allowed
    allowed_client_commands = {CommandType::CMD_HELP};

    // Add the allowed commands
    for (auto &msg : messages)
    {
        allowed_client_commands.push_back(msg);
    }
}

Message UDPPostman::data_to_message(std::vector<uint8_t> data)
{
    if (data.size() < 1)
    {
        Message unknown;
        unknown.type = MessageType::UNKNOWN;
        return unknown;
    }

    MessageType type = (MessageType)data.at(0);
    switch (type)
    {
    case MessageType::CONFIRM:
    {
        Message msg;
        msg.type = MessageType::CONFIRM;
        std::memcpy(&msg.ref_id, &data.at(1), sizeof(MessageID));
        msg.ref_id = ntohs(msg.ref_id);
        return msg;
    }
    case MessageType::REPLY:
    {
        Message msg;
        msg.type = MessageType::REPLY;
        std::memcpy(&msg.id, &data.at(1), sizeof(MessageID));
        msg.id = ntohs(msg.id);
        msg.result = data.at(3);
        std::memcpy(&msg.ref_id, &data.at(4), sizeof(MessageID));
        msg.ref_id = ntohs(msg.ref_id);
        msg.contents = std::string(data.begin() + BEG_OFFSET + STR_OFFSET, data.end());
        return msg;
    }

    case MessageType::AUTH:
    {
        Message msg;
        msg.type = MessageType::AUTH;
        std::memcpy(&msg.id, &data.at(1), sizeof(MessageID));
        msg.id = ntohs(msg.id);
        ssize_t i = BEG_OFFSET;
        while (data.at(i) != 0)
        {
            msg.username.push_back(data.at(i));
            i++;
        }
        i += STR_OFFSET;
        while (data.at(i) != 0)
        {
            msg.display_name.push_back(data.at(i));
            i++;
        }
        i += STR_OFFSET;
        while (data.at(i) != 0)
        {
            msg.password.push_back(data.at(i));
            i++;
        }
        return msg;
    }

    case MessageType::JOIN:
    {
        Message msg;
        msg.type = MessageType::JOIN;
        std::memcpy(&msg.id, &data.at(1), sizeof(MessageID));
        msg.id = ntohs(msg.id);
        ssize_t i = BEG_OFFSET;
        while (data.at(i) != 0)
        {
            msg.channel_id.push_back(data.at(i));
            i++;
        }
        i += STR_OFFSET;
        while (data.at(i) != 0)
        {
            msg.display_name.push_back(data.at(i));
            i++;
        }
        return msg;
    }

    case MessageType::MSG:
    {
        Message msg;
        msg.type = MessageType::MSG;
        std::memcpy(&msg.id, &data.at(1), sizeof(MessageID));
        msg.id = ntohs(msg.id);
        ssize_t i = BEG_OFFSET;
        while (data.at(i) != 0)
        {
            msg.display_name.push_back(data.at(i));
            i++;
        }
        i += STR_OFFSET;
        while (data.at(i) != 0)
        {
            msg.contents.push_back(data.at(i));
            i++;
        }
        return msg;
    }

    case MessageType::ERR:
    {
        Message msg;
        msg.type = MessageType::ERR;
        std::memcpy(&msg.id, &data.at(1), sizeof(MessageID));
        msg.id = ntohs(msg.id);
        ssize_t i = BEG_OFFSET;
        while (data.at(i) != 0)
        {
            msg.display_name.push_back(data.at(i));
            i++;
        }
        i += STR_OFFSET;
        while (data.at(i) != 0)
        {
            msg.contents.push_back(data.at(i));
            i++;
        }
        return msg;
    }

    case MessageType::BYE:
    {
        Message msg;
        msg.type = MessageType::BYE;
        std::memcpy(&msg.id, &data.at(1), sizeof(MessageID));
        msg.id = ntohs(msg.id);
        return msg;
    }

    default:
        Message unknown;
        unknown.type = MessageType::UNKNOWN;
        return unknown;
    }

    Message unknown;
    unknown.type = MessageType::UNKNOWN;
    return unknown;
}

void UDPPostman::check_waiters()
{
    if (confirm_waiters.empty())
    {
        return;
    }

    // Check if any of the confirm_waiters have timed out
    for (auto it = confirm_waiters.begin(); it != confirm_waiters.end(); it++)
    {
        // Update the timestamp of the last check
        timestamp = Utils::get_timestamp();

        // If the waiter has timed out, resend the message
        if (it->expiration < timestamp)
        {
            // If the waiter has tries left, resend the message
            if (it->tries_left > 0)
            {
                std::clog << FBLU("resending message: ") << (int)it->id << std::endl;

                // Send the message
                ssize_t n = sendto(this->get_client_socket(), it->data.data(), it->data.size(), 0, (struct sockaddr *)&server_address, sizeof(server_address));
                // Check for errors
                if (n < 0)
                {
                    throw std::runtime_error("ERROR sending message");
                }

                // Reset the timer and decrement the number of tries
                it->expiration = timestamp + MSG_TIMEOUT;
                it->tries_left--;
            }
            else
            {
                // If the waiter has no more tries left, remove it from the queue
                confirm_waiters.erase(it);
                std::cerr << "ERROR: message not confirmed" << std::endl;
            }
        }
    }
}
