#include "fsm.h"

#define COL_CYAN "\x1b[36m"
#define COL_RESET "\x1b[0m"

Automata::Automata() : postman()
{
    std::cout << "ahoj" << std::endl;
    postman.attach_to_server("localhost", 12000);
    state = S_START;
};

Automata::~Automata()
{
    std::cout << "tak cau" << std::endl;
}

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

void Automata::handle_msg(Message msg)
{
    if (msg.type == CONFIRM)
    {
        std::clog << "(received) " << std::endl;
        return;
    }
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
    std::cout << "\n------- MSG END -------\n"
              << COL_RESET << std::endl;
}

State Automata::open_polling()
{
    // Create a new kqueue
    int kq = kqueue();
    if (kq == -1)
    {
        throw std::runtime_error("Failed to create kqueue.");
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

    // Wait for events to occur
    while (true)
    { // Use proper condition for termination
        std::clog << std::endl
                  << "Waiting for events..." << std::endl;
        Automata::print_leader();

        std::cout << "= stack state: " << postman.message_stack.size() << std::endl;

        // If there are messages in the stack, read them
        if (postman.message_stack.size() > 0)
        {
            std::clog << "READING FROM STACK" << std::endl;

            Message msg = postman.message_stack.top();
            postman.message_stack.pop();

            handle_msg(msg);
            continue;
        }

        // Wait for events on stdin and the UDP socket
        int nev = kevent(kq, NULL, 0, evList, MAX_EVENTS, NULL);
        for (int i = 0; i < nev; i++)
        {
            if (evList[i].ident == (uintptr_t)udp_fd)
            {
                std::cout << std::endl;

                // Receive the message from the server
                Message msg = postman.receive();

                // Check if the message leads to a state change
                if (msg.type == BYE)
                {
                    // Server closed the connection
                    std::clog << "Server closed the connection." << std::endl;

                    close(kq);
                    return S_END;
                }
                else if (msg.type == ERR)
                {
                    // Server sent an error message
                    std::clog << "Server sent an error message." << std::endl;

                    // Send the BYE message to the server
                    postman.bye();
                    // Wait for the response
                    Message res = postman.receive_with_retry(MSG_TIMEOUT, MSG_MAX_RETRIES);

                    close(kq);
                    return S_END;
                }
                else if (msg.type != MSG)
                {
                    // Unknown message type
                    std::clog << "Unknown message type: " << (int)msg.type << std::endl;

                    // Send the ERROR message to the server
                    postman.error("User", "Unknown message type.");
                    // Wait for the response
                    Message res = postman.receive_with_retry(MSG_TIMEOUT, MSG_MAX_RETRIES);

                    close(kq);
                    return S_ERROR;
                }

                // Read the message from the server
                handle_msg(msg);

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

                    close(kq);
                    return S_END;
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
    return S_ERROR;
}

void Automata::run()
{
    while (1)
    {
        std::cout << "\033[93m" << state << "\033[0m" << std::endl;
        switch (state)
        {
        case S_START:
            // TODO: add username, display name and password
            postman.authorize("user", "User", "password");
            state = S_AUTH;
            break;

        case S_AUTH:
        {
            try
            {
                // Wait for the response REPLY message
                Message msg = postman.receive_with_retry(MSG_TIMEOUT, MSG_MAX_RETRIES);

                // Check if the message is a REPLY message and had value 1
                if (msg.type == REPLY && Postman::get_reply(msg))
                {
                    std::clog << "Authorized." << std::endl;
                    state = S_OPEN;
                }
                else
                {
                    std::clog << "Authorization failed." << std::endl;
                    state = S_ERROR;
                }
            }
            catch (...)
            {
                // An error occurred
                state = S_ERROR;
            }
            break;
        }

        case S_OPEN:
        { // Open polling for the automata for the client socket and stdin.
            State next_state = open_polling();

            // Transition to the next state returned by open_polling
            state = next_state;

            break;
        }

        case S_ERROR:
            // An error occurred
            std::clog << "An error occurred." << std::endl;
            // Send the BYE message to the server
            postman.bye();
            // Wait for the response
            state = S_END;

            break;

        case S_END:
            std::cout << "Bye bye!" << std::endl;
            return;

        default:
            break;
        }
    }
}

void Automata::print_leader()
{
    std::clog << "> ";
}