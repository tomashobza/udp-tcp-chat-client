#include "UdpPostman.hpp"

/** Had SIGINT flag */
bool udp_had_sigint = false;

void UDPPostman::udp_handle_sigint(int signal)
{
    if (signal == SIGINT)
    {
        udp_had_sigint = true;
    }
}

UDPPostman::UDPPostman(Args args)
{
    std::signal(SIGINT, UDPPostman::udp_handle_sigint);

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

std::string UDPPostman::get_display_name()
{
    return display_name;
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
    // get the message data size
    size_t data_len = BEG_OFFSET + username.size() + STR_OFFSET + display_name.size() + STR_OFFSET + password.size() + STR_OFFSET;

    // create the message data buffer
    std::vector<uint8_t> data(data_len);
    std::fill(data.begin(), data.end(), 0);
    data[0] = MessageType::AUTH;
    data[1] = (uint8_t)msg_id >> 8;
    data[2] = (uint8_t)msg_id & 0xFF;
    std::memcpy(&data[BEG_OFFSET], username.c_str(), username.size());
    std::memcpy(&data[BEG_OFFSET + username.size() + STR_OFFSET], display_name.c_str(), display_name.size());
    std::memcpy(&data[BEG_OFFSET + username.size() + STR_OFFSET + display_name.size() + STR_OFFSET], password.c_str(), password.size());

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
    // get the message data size
    size_t data_len = BEG_OFFSET + channel_id.size() + STR_OFFSET + display_name.size() + STR_OFFSET;

    // create the message data buffer
    std::vector<uint8_t> data(data_len);
    std::fill(data.begin(), data.end(), 0);
    data[0] = MessageType::JOIN;
    data[1] = (uint8_t)msg_id >> 8;
    data[2] = (uint8_t)msg_id & 0xFF;
    std::memcpy(&data[BEG_OFFSET], channel_id.c_str(), channel_id.size());
    std::memcpy(&data[BEG_OFFSET + channel_id.size() + STR_OFFSET], display_name.c_str(), display_name.size());

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
    // get the message data size
    size_t data_len = BEG_OFFSET + display_name.size() + STR_OFFSET + message_contents.size() + STR_OFFSET;

    // create the message data buffer
    std::vector<uint8_t> data(data_len);
    std::fill(data.begin(), data.end(), 0);

    data[0] = MessageType::MSG;
    data[1] = (uint8_t)msg_id >> 8;
    data[2] = (uint8_t)msg_id & 0xFF;
    std::memcpy(&data[BEG_OFFSET], display_name.c_str(), display_name.size());
    std::memcpy(&data[BEG_OFFSET + display_name.size() + STR_OFFSET], message_contents.c_str(), message_contents.size());

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
    // get the message data size
    size_t data_len = BEG_OFFSET + display_name.size() + STR_OFFSET + message_contents.size() + STR_OFFSET;

    // create the message data buffer
    std::vector<uint8_t> data(data_len);
    std::fill(data.begin(), data.end(), 0);
    data[0] = MessageType::ERR;
    data[1] = (uint8_t)msg_id >> 8;
    data[2] = (uint8_t)msg_id & 0xFF;
    std::memcpy(&data[BEG_OFFSET], display_name.c_str(), display_name.size());
    std::memcpy(&data[BEG_OFFSET + display_name.size() + STR_OFFSET], message_contents.c_str(), message_contents.size());

    // Make sure the message is null-terminated
    if (data.at(data.size() - 1) != '\0')
    {
        data.push_back('\0');
        data_len++;
    }

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
    // get the message data size
    size_t data_len = BEG_OFFSET;

    // create the message data buffer
    std::vector<uint8_t> data(data_len);
    std::fill(data.begin(), data.end(), 0);
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
    // get the message data size
    size_t data_len = BEG_OFFSET;

    // create the message data buffer
    std::vector<uint8_t> data(data_len);
    std::fill(data.begin(), data.end(), 0);
    data[0] = MessageType::CONFIRM;
    data[1] = (uint8_t)ref_msg_id >> 8;
    data[2] = (uint8_t)ref_msg_id & 0xFF;

    // send the message
    ssize_t n = sendto(client_socket, data.data(), data_len, 0,
                       (struct sockaddr *)&server_address, sizeof(server_address));

    // check for errors
    if (n < 0)
    {
        throw std::runtime_error("ERROR sending CONFIRM message");
    }

    return 0;
}

int UDPPostman::confirm(MessageID ref_id)
{
    // get the message data size
    size_t data_len = BEG_OFFSET;

    // create the message data buffer
    std::vector<uint8_t> data(data_len);
    std::fill(data.begin(), data.end(), 0);
    data[0] = MessageType::CONFIRM;
    data[1] = (uint8_t)ref_id >> 8;
    data[2] = (uint8_t)ref_id & 0xFF;

    // send the message
    ssize_t n = sendto(client_socket, data.data(), data_len, 0,
                       (struct sockaddr *)&server_address, sizeof(server_address));

    // check for errors
    if (n < 0)
    {
        throw std::runtime_error("ERROR sending CONFIRM message");
    }

    return 0;
}

PollResults UDPPostman::poll_for_messages()
{
    PollResults results;

    // Check if the user has pressed Ctrl+C
    if (udp_had_sigint)
    {
        is_waiting_for_reply = false;
        // Send the BYE message
        PollResult res;
        res.type = PollResultType::USER;
        res.message.type = MessageType::BYE;
        res.message.id = msg_id;
        results.push_back(res);
        return results;
    }

    // Check if there are any messages to be confirmed
    if (!this->check_waiters())
    {
        is_waiting_for_reply = false;
        PollResult res;
        res.type = PollResultType::USER;
        res.message.type = MessageType::ERR;
        res.message.display_name = display_name;
        res.message.contents = "Message not confirmed";
        results.push_back(res);
        return results;
    }

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
        if (udp_had_sigint)
        {
            is_waiting_for_reply = false;
            PollResult res;
            res.type = PollResultType::USER;
            res.message.type = MessageType::BYE;
            res.message.id = msg_id;
            results.push_back(res);
            return results;
        }
        else
        {
            throw std::runtime_error("ERROR polling for messages");
        }
    }

    // Check if there are any messages to be confirmed
    if (!this->check_waiters())
    {
        is_waiting_for_reply = false;
        PollResult res;
        res.type = PollResultType::USER;
        res.message.type = MessageType::ERR;
        res.message.display_name = display_name;
        res.message.contents = "Message not confirmed";
        results.push_back(res);
        return results;
    }

    bool no_confirm_waiters = confirm_waiters.empty();

    // If stdin has data and there are no messages to be confirmed and the client is not waiting for a reply
    if (fds[0].revents & POLLIN && no_confirm_waiters && !is_waiting_for_reply)
    {
        // Check if stdin is closed
        char c;
        // Try reading one byte from stdin to check if it is closed
        ssize_t bytesRead = read(0, &c, 1);
        if (bytesRead <= 0)
        {
            is_waiting_for_reply = false;
            // Send the BYE message
            PollResult res;
            res.type = PollResultType::USER;
            res.message.type = MessageType::BYE;
            res.message.id = msg_id;
            results.push_back(res);
            return results;
        }
        else
        {
            ungetc(c, stdin);
        }

        PollResults usr_res = handle_user_command();
        for (auto &res : usr_res)
        {
            results.push_back(res);
        }
    }

    // If socket has data
    if (fds[1].revents & POLLIN)
    {
        PollResults srvr_res = handle_server_message();
        for (auto &res : srvr_res)
        {
            results.push_back(res);
        }
    }

    return results;
}

PollResults UDPPostman::handle_user_command()
{
    PollResults results;

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

    return results;
}

PollResults UDPPostman::handle_server_message()
{
    PollResults results;

    // Receive the message
    Message msg = this->receive();

    // Confirm messages will not be forwarded to the FSM as they are not contentful
    bool is_contentful = msg.type != MessageType::CONFIRM;
    // Unknown messages will get forwarded to the FSM to handle
    bool is_unknown = msg.type == MessageType::UNKNOWN;
    //
    bool is_first = first_message;
    bool is_new = (msg.id > ref_msg_id || is_first);

    if (is_contentful)
    {
        confirm(msg.id);
    }

    // Add the message to the results if it is not a confirmation or an unknown message
    if ((is_contentful && is_new) || is_unknown)
    {
        results.push_back(PollResult{
            PollResultType::SERVER,
            msg});

        ref_msg_id = msg.id;
        first_message = false;
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
                    confirm_waiters.erase(it);
                    break;
                }
            }
        }
    }
    else if (msg.type == MessageType::REPLY)
    {
        // Check reference ID of a REPLY message
        if (msg.ref_id == last_sent_message.id)
        {
            // Change the port of the recipient
            if (last_sent_message.type == MessageType::AUTH)
            {
                sockaddr_in *sender_addr = reinterpret_cast<sockaddr_in *>(&new_server_address);
                this->server_address.sin_port = sender_addr->sin_port;
            }

            // Update the waiting flag
            is_waiting_for_reply = false;
        }
        else
        {
            std::cerr << "ERR: Wrong reply RefID!" << std::endl;

            delete buffer; // Important to avoid memory leaks
            Message unknown;
            unknown.type = MessageType::UNKNOWN;
            return unknown;
        }
    }

    return msg;
}

void UDPPostman::allow_client_commands(std::vector<CommandType> messages)
{
    // Help is always allowed
    allowed_client_commands = {CommandType::CMD_HELP, CommandType::CMD_RENAME};

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
        msg.ref_id = data.at(1) << 8 | data.at(2);
        if (data.size() != 3)
        {
            std::cerr << "ERR: CONFIRM message not complete!" << std::endl;
            Message unknown;
            unknown.type = MessageType::UNKNOWN;
            return unknown;
        }
        return msg;
    }
    case MessageType::REPLY:
    {
        Message msg;
        msg.type = MessageType::REPLY;
        msg.id = data.at(1) << 8 | data.at(2);
        msg.result = data.at(3);
        msg.ref_id = data.at(4) << 8 | data.at(5);
        msg.contents = std::string(data.begin() + BEG_OFFSET + STR_OFFSET + sizeof(MessageID), data.end());
        if (msg.contents.at(msg.contents.size() - 1) == '\0')
        {
            msg.contents.pop_back();
        }
        else
        {
            std::cerr << "ERR: REPLY message not null-terminated!" << std::endl;
            Message unknown;
            unknown.type = MessageType::UNKNOWN;
            if (data.size() > 3)
            {
                unknown.id = data.at(1) << 8 | data.at(2);
            }
            return unknown;
        }
        return msg;
    }

    case MessageType::AUTH:
    {
        Message msg;
        msg.type = MessageType::AUTH;
        msg.id = data.at(1) << 8 | data.at(2);
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

        if (msg.username.size() == 0 || msg.display_name.size() == 0 || msg.password.size() == 0 || data.at(i) != 0)
        {
            std::cerr << "ERR: AUTH message has empty fields or wrong NULL termination!" << std::endl;
            Message unknown;
            unknown.type = MessageType::UNKNOWN;
            if (data.size() > 3)
            {
                unknown.id = data.at(1) << 8 | data.at(2);
            }
            return unknown;
        }

        return msg;
    }

    case MessageType::JOIN:
    {
        Message msg;
        msg.type = MessageType::JOIN;
        msg.id = data.at(1) << 8 | data.at(2);
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

        if (msg.channel_id.size() == 0 || msg.display_name.size() == 0 || data.at(i) != 0)
        {
            std::cerr << "ERR: JOIN message has empty fields or wrong NULL termination!" << std::endl;
            Message unknown;
            unknown.type = MessageType::UNKNOWN;
            if (data.size() > 3)
            {
                unknown.id = data.at(1) << 8 | data.at(2);
            }
            return unknown;
        }

        return msg;
    }

    case MessageType::MSG:
    {
        Message msg;
        msg.type = MessageType::MSG;
        msg.id = data.at(1) << 8 | data.at(2);
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

        if (msg.display_name.size() == 0 || msg.contents.size() == 0 || data.at(i) != 0)
        {
            std::cerr << "ERR: MSG message has empty fields or wrong NULL termination!" << std::endl;
            Message unknown;
            unknown.type = MessageType::UNKNOWN;
            if (data.size() > 3)
            {
                unknown.id = data.at(1) << 8 | data.at(2);
            }
            return unknown;
        }

        return msg;
    }

    case MessageType::ERR:
    {
        Message msg;
        msg.type = MessageType::ERR;
        msg.id = data.at(1) << 8 | data.at(2);
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

        if (msg.display_name.size() == 0 || msg.contents.size() == 0 || data.at(i) != 0)
        {
            std::cerr << "ERR: ERR message has empty fields or wrong NULL termination!" << std::endl;
            Message unknown;
            unknown.type = MessageType::UNKNOWN;
            if (data.size() > 3)
            {
                unknown.id = data.at(1) << 8 | data.at(2);
            }
            return unknown;
        }

        return msg;
    }

    case MessageType::BYE:
    {
        Message msg;
        msg.type = MessageType::BYE;
        msg.id = data.at(1) << 8 | data.at(2);
        if (data.size() != 3)
        {
            std::cerr << "ERR: BYE message not complete!" << std::endl;
            Message unknown;
            unknown.type = MessageType::UNKNOWN;
            return unknown;
        }
        return msg;
    }

    default:
        Message unknown;
        unknown.type = MessageType::UNKNOWN;
        unknown.id = data.at(1) << 8 | data.at(2);
        if (data.size() > 3)
        {
            unknown.id = data.at(1) << 8 | data.at(2);
        }
        return unknown;
    }

    Message unknown;
    unknown.type = MessageType::UNKNOWN;
    return unknown;
}

bool UDPPostman::check_waiters()
{
    if (confirm_waiters.empty())
    {
        return true;
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
                // Send the message
                ssize_t n = sendto(this->get_client_socket(), it->data.data(), it->data.size(), 0, (struct sockaddr *)&server_address, sizeof(server_address));
                // Check for errors
                if (n < 0)
                {
                    throw std::runtime_error("ERR: sending message");
                }

                // Reset the timer and decrement the number of tries
                it->expiration = timestamp + MSG_TIMEOUT;
                it->tries_left--;
            }
            else
            {
                // If the waiter has no more tries left, remove it from the queue
                confirm_waiters.erase(it);
                return false;
            }
        }
    }

    return true;
}
