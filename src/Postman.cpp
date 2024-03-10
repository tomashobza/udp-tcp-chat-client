#include "Postman.hpp"

int IPostman::get_client_socket()
{
    return client_socket;
}

struct sockaddr_in IPostman::get_server_address()
{
    return server_address;
}