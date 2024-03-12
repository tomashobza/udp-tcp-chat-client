#include "Automata.hpp"

Automata::Automata(Args args)
{
    // Choose the desired protocol and create the postman
    switch (args.type)
    {
    case Protocol::UDP:
        postman = new UDPPostman(args);
        break;
    // case Protocol::TCP:
    //     postman = new TCPPostman(args);
    //     break;
    default:
        break;
    }

    postman->attach_to_server(args.hostname, args.port);

    state = S_START;
};

Automata::~Automata(){};

State Automata::set_state(State new_state)
{
    state = new_state;
    std::clog << "\033[0;33m#";
    switch (state)
    {
    case S_START:
        std::clog << " S_START";
        break;
    case S_AUTH:
        std::clog << " S_AUTH";
        break;
    case S_OPEN:
        std::clog << " S_OPEN";
        break;
    case S_ERROR:
        std::clog << " S_ERROR";
        break;
    case S_END:
        std::clog << " S_END";
        break;
    default:
        break;
    }
    std::clog << "\033[0m" << std::endl;
    return state;
};

void Automata::rename(const std::string &new_display_name)
{
    display_name = new_display_name;
};

void Automata::run()
{
    // Run the automata
    while (1)
    {
        switch (state)
        {
        case S_START:
            s_start();
            break;
        case S_AUTH:
            s_auth();
            break;
        case S_OPEN:
            s_open();
            break;
        case S_ERROR:
            s_error();
            break;
        case S_END:
            s_end();
            return;
            break;
        default:
            s_error();
            break;
        }
    }

    return;
};

State Automata::s_start()
{
    // Poll for messages
    PollResults results = postman->poll_for_messages();

    for (auto &res : results)
    {
        // Handle the user message (ignore server messages)
        if (res.type == PollResultType::USER)
        {
            // Parse the message
            if (res.message.type == MessageType::AUTH)
            {
                Message auth = res.message;
                // Authorize the user
                postman->authorize(auth.username, display_name, auth.password);
                set_state(S_AUTH);
            }
            else
            {
                std::cerr << "Invalid message type!" << std::endl;
            }
        }
    }

    return state;
}

State Automata::s_auth()
{
    // Poll for messages
    PollResults results = postman->poll_for_messages();

    for (auto &res : results)
    {
        // Handle the user message (ignore server messages)
        if (res.type == PollResultType::SERVER)
        {
            std::cout << "Received server message: " << (int)res.message.type << std::endl;
            if (res.message.type == MessageType::REPLY)
            {
                // TODO: check id
                std::cout << "Authorization successful!" << std::endl;
                set_state(S_OPEN);
            }
            else
            {
                std::cout << "Authorization failed!" << std::endl;
                set_state(S_ERROR);
            }
        }
    }

    return state;
}

State Automata::s_open()
{
    // Poll for messages
    PollResults results = postman->poll_for_messages();

    for (auto &res : results)
    {
        if (res.type == PollResultType::USER)
        {
            if (res.message.type == MessageType::MSG)
            {
                // Send the message
                postman->message(display_name, res.message.contents);
            }
        }

        // TODO: continue here
    }

    return state;
}

State Automata::s_error()
{
    return state;
}

State Automata::s_end()
{
    std::cout << "Goodbye!" << std::endl;
    return S_END;
}
