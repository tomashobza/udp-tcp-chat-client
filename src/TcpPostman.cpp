// #include "TcpPostman.hpp"

// TCPPostman::TCPPostman(Args args)
// {
//     // Create the socket
//     client_socket = socket(AF_INET, SOCK_STREAM, 0);
//     if (client_socket < 0)
//     {
//         throw std::runtime_error("ERROR opening socket");
//     }

//     // Attach to the server
//     this->attach_to_server(args.hostname, args.port);
// }

// TCPPostman::~TCPPostman()
// {
//     close(client_socket);
// }

// void TCPPostman::attach_to_server(const std::string &server_hostname, uint16_t port_number)
// {
//     // Set the server address
//     std::memset(&server_address, 0, sizeof(server_address));

//     // Get the server by its hostname
//     struct hostent *server = gethostbyname(server_hostname.c_str());
//     if (server == nullptr)
//     {
//         throw std::runtime_error("ERROR, no such host as '" + server_hostname + "'");
//     }

//     // Set the server address
//     server_address.sin_family = AF_INET;
//     std::memcpy(&server_address.sin_addr.s_addr, server->h_addr, server->h_length);
//     server_address.sin_port = htons(port_number);

//     // Connect to the server
//     if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
//     {
//         throw std::runtime_error("ERROR connecting to server");
//     }
// }

// // int TCPPostman::authorize(const std::string &username, const std::string &display_name, const std::string &password)
// // {
// //     // TODO: Implement this function
// //     return 0;
// // }

// // int TCPPostman::join(const std::string &channel_id, const std::string &display_name)
// // {
// //     // TODO: Implement this function
// //     return 0;
// // }

// // int TCPPostman::message(const std::string &display_name, const std::string &message_contents)
// // {
// //     // TODO: Implement this function
// //     return 0;
// // }

// // int TCPPostman::error(const std::string &display_name, const std::string &message_contents)
// // {
// //     // TODO: Implement this function
// //     return 0;
// // }

// // int TCPPostman::bye()
// // {
// //     // TODO: Implement this function
// //     return 0;
// // }

// // int TCPPostman::confirm()
// // {
// //     // TODO: Implement this function
// //     return 0;
// // }

// // int TCPPostman::get_client_socket()
// // {
// //     return client_socket;
// // }

// // struct sockaddr_in TCPPostman::get_server_address()
// // {
// //     return server_address;
// // }

// // PollResults TCPPostman::poll_for_messages()
// // {
// //     // TODO: Implement this function
// // }
