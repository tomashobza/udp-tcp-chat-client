#include "fsm.h"

#define COL_CYAN "\x1b[36m"
#define COL_RESET "\x1b[0m"

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
    // Read from stdin
    char buffer[READ_SIZE];
    int n = read(STDIN_FILENO, buffer, READ_SIZE);
    if (n == -1)
    {
        throw std::runtime_error("Failed to read from stdin.");
        return "";
    }

    // Remove the newline character
    if (buffer[n - 1] == '\n')
    {
        buffer[n - 1] = 0;
        n--;
    }

    // Return the string
    return std::string(buffer, n);
}

void Automata::handle_msg()
{
    Message msg = postman.receive_with_retry(MSG_TIMEOUT, MSG_MAX_RETRIES);
    std::cout << COL_CYAN << "------- MSG START -------\n"
              << std::endl;
    std::cout << "UDP: " << (int)msg.type << ":" << msg.id << std::endl;
    // print the rest of the message as two strings separated by character of value 0
    for (size_t i = 3; i < msg.data.size(); i++)
    {
        if (msg.data.at(i) == 0)
        {
            std::cout << std::endl;
        }
        else
        {
            std::cout << msg.data.at(i);
        }
    }
    std::cout << "------- MSG END -------\n"
              << COL_RESET << std::endl;
}

void Automata::open_polling()
{
    // Create a new kqueue
    int kq = kqueue();
    if (kq == -1)
    {
        std::cerr << "Failed to create kqueue.\n";
        return;
    }

    // Set up the kevent structure
    struct kevent evSet;
    struct kevent evList[MAX_EVENTS];

    int udp_fd = postman.get_client_socket(); // Get the UDP socket

    // Monitor UDP socket for reading
    EV_SET(&evSet, udp_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
    kevent(kq, &evSet, 1, NULL, 0, NULL);

    // Monitor stdin for reading
    EV_SET(&evSet, STDIN_FILENO, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
    kevent(kq, &evSet, 1, NULL, 0, NULL);

    Automata::print_leader();

    // Wait for events to occur
    while (true)
    { // Use proper condition for termination
        int nev = kevent(kq, NULL, 0, evList, MAX_EVENTS, NULL);
        for (int i = 0; i < nev; i++)
        {
            if (evList[i].ident == (uintptr_t)udp_fd)
            {
                // Read the message from the server
                handle_msg();

                // TODO: Handle the message

                Automata::print_leader();
            }
            else if (evList[i].ident == STDIN_FILENO)
            {
                // Read from stdin
                std::string msg = read_stdin();

                if (msg.empty() || msg == "exit")
                {
                    // Send the BYE message to the server
                    postman.bye();
                    return;
                }

                // Send the message to the server
                postman.message("user", msg);

                // Wait for the response
                Message res = postman.receive_with_retry(MSG_TIMEOUT, MSG_MAX_RETRIES);

                std::cout << "(received) " << res.type << std::endl;
            }
        }
    }

    // Cleanup omitted for brevity
    close(kq);
    return;
}

void Automata::print_leader()
{
    std::clog << "> ";
}