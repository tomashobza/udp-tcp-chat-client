#include "fsm.h"

Automata::Automata() : postman()
{
    std::cout << "ahoj" << std::endl;
    postman.attach_to_server("localhost", 12000);
};

Automata::~Automata()
{
    std::cout << "tak cau" << std::endl;
}

/// DEV ///

#define MAX_EVENTS 5
#define READ_SIZE (int)1024

std::string Automata::read_stdin()
{
    char buffer[READ_SIZE];
    int n = read(STDIN_FILENO, buffer, READ_SIZE);
    if (n == -1)
    {
        throw std::runtime_error("Failed to read from stdin.");
        return "";
    }
    return std::string(buffer, n);
}

void Automata::open_polling()
{
    int kq = kqueue();
    if (kq == -1)
    {
        std::cerr << "Failed to create kqueue.\n";
        return;
    }

    struct kevent evSet;
    struct kevent evList[MAX_EVENTS];

    int udp_fd = postman.get_client_socket(); // Make sure this is correctly implemented

    // Monitor UDP socket for reading
    EV_SET(&evSet, udp_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
    kevent(kq, &evSet, 1, NULL, 0, NULL);

    // Monitor stdin for reading
    EV_SET(&evSet, STDIN_FILENO, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
    kevent(kq, &evSet, 1, NULL, 0, NULL);

    while (true)
    { // Use proper condition for termination
        int nev = kevent(kq, NULL, 0, evList, MAX_EVENTS, NULL);
        for (int i = 0; i < nev; i++)
        {
            if (evList[i].ident == (uintptr_t)udp_fd)
            {
                std::vector<uint8_t> msg = postman.receive_with_retry(MSG_TIMEOUT, MSG_MAX_RETRIES);
                std::cout << "------- MSG START -------" << std::endl;
                std::cout << "UDP: " << (int)msg.at(0) << ":" << (int)msg.at(1) + (int)msg.at(2) << std::endl;
                // print the rest of the message as two strings separated by character of value 0
                for (size_t i = 3; i < msg.size(); i++)
                {
                    if (msg.at(i) == 0)
                    {
                        std::cout << std::endl;
                    }
                    else
                    {
                        std::cout << msg.at(i);
                    }
                }
                std::cout << "------- MSG END -------" << std::endl;
            }
            else if (evList[i].ident == STDIN_FILENO)
            {
                // Read from stdin
                std::string msg = read_stdin();

                // Send the message to the server
                postman.message("user", msg);

                // Wait for the response
                std::vector<uint8_t> res = postman.receive_with_retry(MSG_TIMEOUT, MSG_MAX_RETRIES);
            }
        }
    }

    // Cleanup omitted for brevity
    close(kq);
    return;
}