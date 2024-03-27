#include "TcpPostman.hpp"

TCPPostman::TCPPostman(Args args)
{
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
    std::cout << FBLU("Waiting for reply...") << std::endl;

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
    std::cout << FBLU("Waiting for reply...") << std::endl;

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

int TCPPostman::get_client_socket()
{
    return client_socket;
}

struct sockaddr_in TCPPostman::get_server_address()
{
    return server_address;
}
