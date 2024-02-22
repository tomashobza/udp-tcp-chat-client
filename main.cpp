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

    // Receive a message from the server
    std::vector<uint8_t> buffer = postman.receive_with_retry(MSG_TIMEOUT, MSG_MAX_RETRIES);

    // print first byte of buffer
    std::cout << "First byte of buffer: " << (int)buffer.at(0) << std::endl;

    return 0;
}
