/**
 * @file Postman.cpp
 * @author Tomáš Hobza (xhobza03)
 * @brief Postman class for the project
 * @date 2024-03-31
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "Postman.hpp"

int IPostman::get_client_socket()
{
    return client_socket;
}

struct sockaddr_in IPostman::get_server_address()
{
    return server_address;
}
