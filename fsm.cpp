#include "fsm.h"

#define COL_CYAN "\x1b[36m"
#define COL_RESET "\x1b[0m"

Automata::Automata() : postman()
{
    // TODO: add protocol seleciton
    postman = new UDPPostman();

    postman->attach_to_server("localhost", 4567);
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
    // // Create a new kqueue
    // int kq = kqueue();
    // if (kq == -1)
    // {
    //     throw std::runtime_error("Failed to create kqueue.");
    // }

    // // Set up the kevent structure
    // struct kevent evSet;
    // struct kevent evList[MAX_EVENTS];

    // int udp_fd = postman->get_client_socket(); // Get the UDP socket

    // // Monitor UDP socket for reading
    // EV_SET(&evSet, udp_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
    // kevent(kq, &evSet, 1, NULL, 0, NULL);

    // // Monitor stdin for reading
    // EV_SET(&evSet, STDIN_FILENO, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
    // kevent(kq, &evSet, 1, NULL, 0, NULL);

    // // Wait for events to occur
    // while (true)
    // { // Use proper condition for termination
    //     std::clog << std::endl
    //               << "Waiting for events..." << std::endl;
    //     Automata::print_leader();

    //     std::cout << "= stack state: " << postman->message_stack.size() << std::endl;

    //     // If there are messages in the stack, read them
    //     if (postman->message_stack.size() > 0)
    //     {
    //         std::clog << "READING FROM STACK" << std::endl;

    //         Message msg = postman->message_stack.top();
    //         postman->message_stack.pop();

    //         handle_msg(msg);
    //         continue;
    //     }

    //     // Wait for events on stdin and the UDP socket
    //     int nev = kevent(kq, NULL, 0, evList, MAX_EVENTS, NULL);
    //     for (int i = 0; i < nev; i++)
    //     {
    //         if (evList[i].ident == (uintptr_t)udp_fd)
    //         {
    //             std::cout << std::endl;

    //             // Receive the message from the server
    //             Message msg = postman->receive();

    //             // Check if the message leads to a state change
    //             if (msg.type == BYE)
    //             {
    //                 // Server closed the connection
    //                 std::clog << "Server closed the connection." << std::endl;

    //                 close(kq);
    //                 return S_END;
    //             }
    //             else if (msg.type == ERR)
    //             {
    //                 // Server sent an error message
    //                 std::clog << "Server sent an error message." << std::endl;

    //                 // Send the BYE message to the server
    //                 postman->bye();
    //                 // Wait for the response
    //                 Message res = postman->receive_with_retry(MSG_TIMEOUT, MSG_MAX_RETRIES);

    //                 close(kq);
    //                 return S_END;
    //             }
    //             else if (msg.type != MSG)
    //             {
    //                 // Unknown message type
    //                 std::clog << "Unknown message type: " << (int)msg.type << std::endl;

    //                 // Send the ERROR message to the server
    //                 postman->error("User", "Unknown message type.");
    //                 // Wait for the response
    //                 Message res = postman->receive_with_retry(MSG_TIMEOUT, MSG_MAX_RETRIES);

    //                 close(kq);
    //                 return S_ERROR;
    //             }

    //             // Read the message from the server
    //             handle_msg(msg);

    //             Automata::print_leader();
    //         }
    //         else if (evList[i].ident == STDIN_FILENO)
    //         {
    //             // Read from stdin
    //             std::string msg = read_stdin();

    //             if (msg.empty() || msg == "exit")
    //             {
    //                 // Send the BYE message to the server
    //                 postman->bye();

    //                 close(kq);
    //                 return S_END;
    //             }

    //             // Send the message to the server
    //             postman->message("user", msg);

    //             // Wait for the response
    //             Message res = postman->receive_with_retry(MSG_TIMEOUT, MSG_MAX_RETRIES);

    //             std::cout << "(received) " << res.type << std::endl;
    //         }
    //     }
    // }

    // // Cleanup omitted for brevity
    // close(kq);
    // return S_ERROR;
}

void Automata::run()
{
    // while (1)
    // {
    //     std::cout << "\033[93m" << state << "\033[0m" << std::endl;
    //     switch (state)
    //     {
    //     case S_START:
    //         // TODO: add username, display name and password
    //         postman->authorize("user", "User", "password");
    //         state = S_AUTH;
    //         break;

    //     case S_AUTH:
    //     {
    //         try
    //         {
    //             // Wait for the response REPLY message
    //             Message msg = postman->receive_with_retry(MSG_TIMEOUT, MSG_MAX_RETRIES);

    //             // Check if the message is a REPLY message and had value 1
    //             if (msg.type == REPLY && IPostman::get_reply(msg))
    //             {
    //                 std::clog << "Authorized." << std::endl;
    //                 state = S_OPEN;
    //             }
    //             else
    //             {
    //                 std::clog << "Authorization failed." << std::endl;
    //                 state = S_ERROR;
    //             }
    //         }
    //         catch (...)
    //         {
    //             // An error occurred
    //             state = S_ERROR;
    //         }
    //         break;
    //     }

    //     case S_OPEN:
    //     { // Open polling for the automata for the client socket and stdin.
    //         State next_state = open_polling();

    //         // Transition to the next state returned by open_polling
    //         state = next_state;

    //         break;
    //     }

    //     case S_ERROR:
    //         // An error occurred
    //         std::clog << "An error occurred." << std::endl;
    //         // Send the BYE message to the server
    //         postman->bye();
    //         // Wait for the response
    //         state = S_END;

    //         break;

    //     case S_END:
    //         std::cout << "Bye bye!" << std::endl;
    //         return;

    //     default:
    //         break;
    //     }
    // }
}

void Automata::print_leader()
{
    std::clog << "> ";
}

State Automata::set_state(State new_state)
{
    state = new_state;

    std::clog << "? ";
    switch (state)
    {
    case S_START:
        std::clog << "S_START";
        break;
    case S_AUTH:
        std::clog << "S_AUTH";
        break;
    case S_OPEN:
        std::clog << "S_OPEN";
        break;
    case S_ERROR:
        std::clog << "S_ERROR";
        break;
    case S_END:
        std::clog << "S_END";
        break;
    default:
        std::clog << "UNKNOWN";
        break;
    }

    std::clog << std::endl;

    return state;
}

void Automata::start_yaaping()
{
    // Set up poll to listen on stdin and the socket
    struct pollfd fds[2];
    // Listen to stdin
    fds[0].fd = STDIN_FILENO; // stdin
    fds[0].events = POLLIN;
    // Listen to socket
    fds[1].fd = postman->get_client_socket();
    fds[1].events = POLLIN;

    while (state != S_END)
    {
        // Start the timer
        postman->start_timer();

        // Poll for events
        int ret = poll(fds, 2, 100);
        if (ret < 0)
        {
            perror("poll");
            exit(EXIT_FAILURE);
        }

        // Check messages waiting to be confirmed
        postman->check_waiters();

        // Check state changes not dependent on input
        switch (state)
        {
        case S_ERROR:
            // Send the BYE message to the server
            postman->bye();
            this->set_state(S_END);
            break;

        // All other states are handled in the main loop
        default:
            break;
        }

        // If stdin has data
        if (fds[0].revents & POLLIN)
        {
            std::string input;
            std::getline(std::cin, input);
            // delete the last character if it is a newline
            if (input.back() == '\n')
            {
                input.pop_back();
            }

            // If the input is empty or "exit", break the loop
            if (input.empty() || input == "exit")
            {
                break;
            }

            std::vector<std::string> input_tokens = InputParser::tokenize_input(input);
            for (auto token : input_tokens)
            {
                std::cout << "Token: " << token << std::endl;
            }

            // TODO: handle input_tokens.size == 0

            Command cmd = InputParser::get_command(input_tokens.at(0));

            // Next state logic for stdin input
            switch (state)
            {
            case S_START:
                if (cmd == CMD_AUTH)
                {
                    if (input_tokens.size() != 4)
                    {
                        throw std::runtime_error("Invalid number of arguments for /auth command.");
                    }
                    postman->authorize(input_tokens.at(1), input_tokens.at(3), input_tokens.at(2));
                    this->set_state(S_AUTH);
                }
                else
                {
                    this->set_state(S_ERROR);
                }
                break;

            case S_AUTH:
                // User input is not valid in this state
                this->set_state(S_ERROR);
                break;

            case S_OPEN:
                // TODO: handle JOIN and MSG
                break;

            case S_ERROR:
                // Should not receive messages in this state
                break;

            case S_END:
                // This will never happen as the loop will break
                break;

            default:
                this->set_state(S_ERROR);
                break;
            };
        }

        // If socket has data
        if (fds[1].revents & POLLIN)
        {
            // Read the message from the server
            Message msg = postman->receive();

            // Non-confirm messages are confirmed immediately
            if (msg.type != MessageType::CONFIRM)
            {
                postman->confirm();
            }

            // Check if the message leads to a state change
            switch (state)
            {
            case S_START:
                // Server sent a message in the wrong state
                this->set_state(S_ERROR);
                break;

            case S_AUTH:
                // TODO: handle REPLY, any other message is an error
                break;

            case S_OPEN:
                // TODO: handle MSG, REPLY, ERR, BYE, any other message is an error
                break;

            case S_ERROR:
                // Should not receive messages in this state
                this->set_state(S_ERROR);
                break;

            case S_END:
                // This will never happen as the loop will break
                break;

            default:
                this->set_state(S_ERROR);
                break;
            };
        }
    }

    std::cout << "👋 Bye Bye!" << std::endl;
    close(postman->get_client_socket());
}