#include <iostream>
#include <sys/select.h>
#include "postman.cpp"

int main()
{
    // Create a new Postman object
    Postman postman;

    // Attach Postman to the server
    postman.attach_to_server("localhost", 12000);

    // Authorize the user
    postman.authorize("user", "User", "password");
    std::vector<uint8_t> buffer = postman.receive();
    // print first byte of buffer
    std::cout << "First byte of buffer: " << (int)buffer.at(0) << std::endl;

    // int fd = postman.get_client_socket();
    // while (1)
    // {
    //     int retval;
    //     fd_set rfds;
    //     // one second timeout
    //     struct timeval tv = {1, 0};

    //     FD_ZERO(&rfds);
    //     FD_SET(fd, &rfds);

    //     retval = select(1, &rfds, NULL, NULL, &tv);

    //     if (retval == -1)
    //     {
    //         throw std::runtime_error("ERROR select()");
    //     }
    //     else if (retval)
    //     {
    //         std::cout << "Data is available now." << std::endl;
    //         break;
    //     }
    //     else
    //     {
    //         std::cout << "Waiting for data..." << std::endl;
    //     }
    // }

    // // Join a channel
    // postman.join("channel", "User");

    // // Send a message
    // postman.message("User", "Hello, world!");

    // // Send an error message
    // postman.error("User", "This is an error message");

    // // Send a bye message
    // postman.bye();

    return 0;
}
