#include <iostream>
#include "postman.cpp"

int main()
{
    // Create a new Postman object
    Postman postman;
    if (postman.get_client_socket() < 0)
    {
        std::cerr << "Error creating client socket" << std::endl;
        return -1;
    }

    // Attach Postman to the server
    try
    {
        postman.attach_to_server("localdsahost", 12000);
    }
    catch (const std::runtime_error &error)
    {
        std::cerr << error.what() << std::endl;
        return -1;
    }

    // Send a test message
    if (postman.test_send() < 0)
    {
        std::cerr << "Error sending message" << std::endl;
        return -1;
    }

    return 0;
}
