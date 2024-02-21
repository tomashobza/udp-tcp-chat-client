#include <iostream>
#include <cstring>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdexcept>

/**
 * @brief A class for sending messages to a server.
 */
class Postman
{
private:
    /** Client side socket file descriptor */
    int client_socket;
    /** Server address */
    struct sockaddr_in server_address;

public:
    /**
     * @brief Construct a new Postman object and create a socket.
     *
     */
    Postman()
    {
        client_socket = socket(AF_INET, SOCK_DGRAM, 0);
        std::clog << "Created client socket: " << client_socket << std::endl;
    }

    /**
     * @brief Attach to a server by hostname and port number.
     *
     * @param server_hostname - the hostname of the server
     * @param port_number - the port number of the server
     */
    void attach_to_server(std::string server_hostname, uint16_t port_number)
    {
        std::memset(&server_address, 0, sizeof(server_address));

        // Get the server by name
        struct hostent *server = gethostbyname(server_hostname.c_str());
        if (server == nullptr)
        {
            throw std::runtime_error("ERROR, no such host as '" + server_hostname + "'");
        }

        server_address.sin_family = AF_INET;
        // Copy the server's IP address to the sockaddr_in structure
        std::memcpy(&server_address.sin_addr.s_addr, server->h_addr, server->h_length);
        server_address.sin_port = htons(static_cast<uint16_t>(port_number));
    }

    /**
     * @brief Send a test "Hello, world!" message to the server.
     *
     * @return int - 0 if successful, -1 if failed
     */
    int test_send()
    {
        std::string message = "Hello, world!";
        int n = sendto(client_socket, message.c_str(), message.length(), 0, (struct sockaddr *)&server_address, sizeof(server_address));
        if (n < 0)
        {
            std::cerr << "Error sending message" << std::endl;
            return -1;
        }
        return 0;
    }

    /**
     * @brief Get the client socket file descriptor.
     *
     * @return int - the client socket file descriptor
     */
    int get_client_socket()
    {
        return client_socket;
    }

    /**
     * @brief Get the server address.
     *
     * @return struct sockaddr_in - the server address
     */
    struct sockaddr_in get_server_address()
    {
        return server_address;
    }
};