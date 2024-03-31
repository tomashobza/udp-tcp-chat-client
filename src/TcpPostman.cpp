#include "TcpPostman.hpp"

/** Had SIGINT flag */
bool tcp_had_sigint = false;

void TCPPostman::tcp_handle_sigint(int signal)
{
    if (signal == SIGINT)
    {
        tcp_had_sigint = true;
    }
}

TCPPostman::TCPPostman(Args args)
{
    std::signal(SIGINT, TCPPostman::tcp_handle_sigint);

    // Create the socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0)
    {
        throw std::runtime_error("ERROR opening socket");
    }

    // Attach to the server
    this->attach_to_server(args.hostname, args.port);

    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        throw std::runtime_error("ERROR connecting to the TCP server");
    }
}

TCPPostman::~TCPPostman()
{
    close(client_socket);
}

std::string TCPPostman::get_display_name()
{
    return display_name;
}

void TCPPostman::attach_to_server(const std::string &server_hostname, uint16_t port_number)
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

int TCPPostman::authorize(const std::string &username, const std::string &display_name, const std::string &password)
{
    // Assemble the message body
    std::string message = "AUTH " + username + " AS " + display_name + " USING " + password + "\r\n";

    // Send the message
    int n = send(client_socket, message.c_str(), message.length(), 0);

    // check for errors
    if (n < 0)
    {
        throw std::runtime_error("ERROR sending AUTH message");
    }

    last_sent_message = Message{};
    last_sent_message.type = MessageType::AUTH;
    last_sent_message.username = username;
    last_sent_message.display_name = display_name;
    last_sent_message.password = password;

    // Disable user input after joining a channel until a reply is received
    is_waiting_for_reply = true;

    return 0;
}

int TCPPostman::join(const std::string &channel_id, const std::string &display_name)
{
    // Assemble the message body
    std::string message = "JOIN " + channel_id + " AS " + display_name + "\r\n";

    // Send the message
    int n = send(client_socket, message.c_str(), message.length(), 0);

    // check for errors
    if (n < 0)
    {
        throw std::runtime_error("ERROR sending JOIN message");
    }

    last_sent_message = Message{};
    last_sent_message.type = MessageType::JOIN;
    last_sent_message.channel_id = channel_id;
    last_sent_message.display_name = display_name;

    // Disable user input after joining a channel until a reply is received
    is_waiting_for_reply = true;

    return 0;
}

int TCPPostman::message(const std::string &display_name, const std::string &message_contents)
{
    // Assemble the message body
    std::string message = "MSG FROM " + display_name + " IS " + message_contents + "\r\n";

    // Send the message
    int n = send(client_socket, message.c_str(), message.length(), 0);

    // check for errors
    if (n < 0)
    {
        throw std::runtime_error("ERROR sending MSG message");
    }

    last_sent_message = Message{};
    last_sent_message.type = MessageType::MSG;
    last_sent_message.display_name = display_name;
    last_sent_message.contents = message_contents;

    return 0;
}

int TCPPostman::error(const std::string &display_name, const std::string &message_contents)
{
    // Assemble the message body
    std::string message = "ERR FROM " + display_name + " IS " + message_contents + "\r\n";

    // Send the message
    int n = send(client_socket, message.c_str(), message.length(), 0);

    // check for errors
    if (n < 0)
    {
        throw std::runtime_error("ERROR sending MSG message");
    }

    last_sent_message = Message{};
    last_sent_message.type = MessageType::ERR;
    last_sent_message.display_name = display_name;
    last_sent_message.contents = message_contents;

    return 0;
}

int TCPPostman::bye()
{
    // Assemble the message body
    std::string message = "BYE\r\n";

    // Send the message
    int n = send(client_socket, message.c_str(), message.length(), 0);

    // check for errors
    if (n < 0)
    {
        throw std::runtime_error("ERROR sending BYE message");
    }

    last_sent_message = Message{};
    last_sent_message.type = MessageType::BYE;

    return 0;
}

PollResults TCPPostman::poll_for_messages()
{
    PollResults results;

    // Check if the user has pressed Ctrl+C
    if (tcp_had_sigint)
    {
        is_waiting_for_reply = false;
        // Send the BYE message
        PollResult res;
        res.type = PollResultType::USER;
        res.message.type = MessageType::BYE;
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
        if (tcp_had_sigint)
        {
            is_waiting_for_reply = false;
            PollResult res;
            res.type = PollResultType::USER;
            res.message.type = MessageType::BYE;
            results.push_back(res);
            return results;
        }
        else
        {
            throw std::runtime_error("ERROR polling for messages");
        }
    }

    // If stdin has data and there are no messages to be confirmed and the client is not waiting for a reply
    if (fds[0].revents & POLLIN && !is_waiting_for_reply)
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

PollResults TCPPostman::handle_user_command()
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

PollResults TCPPostman::handle_server_message()
{
    PollResults results;

    // Receive the message
    Message msg = this->receive();

    // Confirm messages will not be forwarded to the FSM as they are not contentful
    bool is_contentful = msg.type != MessageType::CONFIRM;
    // Unknown messages will get forwarded to the FSM to handle
    bool is_unknown = msg.type == MessageType::UNKNOWN;

    // Add the message to the results if it is not a confirmation or an unknown message
    if ((is_contentful) || is_unknown)
    {
        results.push_back(PollResult{
            PollResultType::SERVER,
            msg});
    }

    return results;
}

Message TCPPostman::receive()
{
    // Allocate a buffer with a maximum expected size
    size_t maxBufferSize = 2048; // Adjust this size according to your needs
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

    // Check the message type
    if (msg.type == MessageType::REPLY)
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

    return msg;
}

void TCPPostman::allow_client_commands(std::vector<CommandType> messages)
{
    // Help is always allowed
    allowed_client_commands = {CommandType::CMD_HELP, CommandType::CMD_RENAME};

    // Add the allowed commands
    for (auto &msg : messages)
    {
        allowed_client_commands.push_back(msg);
    }
}

Message TCPPostman::data_to_message(std::vector<uint8_t> data)
{
    // Check data size being zero
    if (data.size() < 1)
    {
        Message unknown;
        unknown.type = MessageType::UNKNOWN;
        return unknown;
    }

    // Create a string object from raw data
    std::string data_str(data.begin(), data.end());

    std::smatch matches; // To store the matches found by std::regex_search

    // Regex patterns for message bodies (type, number of values, patter)[]
    std::vector<std::tuple<MessageType, unsigned int, std::regex>> patterns = {
        {MessageType::JOIN, 1, std::regex("JOIN ([A-Za-z0-9-]{1,20}) AS ([\\x21-\\x7E]{1,20})\\r\\n")},
        {MessageType::AUTH, 3, std::regex("AUTH ([A-Za-z0-9-]{1,20}) AS ([\\x21-\\x7E]{1,20}) USING ([A-Za-z0-9-]{1,128})\\r\\n")},
        {MessageType::MSG, 2, std::regex("MSG FROM ([\\x21-\\x7E]{1,20}) IS ([\\x20-\\x7E]{1,1400})\\r\\n")},
        {MessageType::ERR, 2, std::regex("ERR FROM ([\\x21-\\x7E]{1,20}) IS ([\\x20-\\x7E]{1,1400})\\r\\n")},
        {MessageType::REPLY, 2, std::regex("REPLY (OK|NOK) IS ([\\x20-\\x7E]{1,1400})\\r\\n")},
        {MessageType::BYE, 0, std::regex("BYE\\r\\n")},
    };

    // Try to match every pattern on the incoming message
    for (const auto &[type, expectedGroups, pattern] : patterns)
    {
        std::smatch matches;
        if (std::regex_search(data_str, matches, pattern))
        {
            // Check if the number of matches (minus the full match at index 0) matches the expected
            if (matches.size() - 1 == expectedGroups)
            {
                Message msg;

                // Assemble the msg object from the matches
                switch (type)
                {
                case MessageType::JOIN:
                    msg.type = MessageType::JOIN;
                    msg.channel_id = matches[1].str();
                    msg.display_name = matches[2].str();
                    return msg;
                    break;
                case MessageType::AUTH:
                    msg.type = MessageType::AUTH;
                    msg.username = matches[1].str();
                    msg.display_name = matches[2].str();
                    msg.password = matches[3].str();
                    return msg;
                    break;
                case MessageType::MSG:
                    msg.type = MessageType::MSG;
                    msg.display_name = matches[1].str();
                    msg.contents = matches[2].str();
                    return msg;
                    break;
                case MessageType::ERR:
                    msg.type = MessageType::ERR;
                    msg.display_name = matches[1].str();
                    msg.contents = matches[2].str();
                    return msg;
                    break;
                case MessageType::REPLY:
                    // Check for valid REPLY result string
                    if (matches[1].str() == "OK")
                    {
                        msg.result = 1;
                    }
                    else if (matches[1].str() == "NOK")
                    {
                        msg.result = 0;
                    }
                    else
                    {
                        break;
                    }
                    msg.type = MessageType::REPLY;
                    msg.contents = matches[2].str();
                    return msg;
                    break;
                case MessageType::BYE:
                    msg.type = MessageType::BYE;
                    return msg;
                    break;
                default:
                    break;
                }
            }
        }
    }

    // If at no point a valid Message object was returned, the incoming message cannot be parsed
    Message unknown;
    unknown.type = MessageType::UNKNOWN;
    return unknown;
}
