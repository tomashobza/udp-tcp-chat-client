#include "Automata.h"

Automata::Automata(Args args)
{
    // Choose the desired protocol and create the postman
    switch (args.type)
    {
    case Protocol::UDP:
        postman = new UDPPostman(args);
        break;
    case Protocol::TCP:
        postman = new TCPPostman(args);
        break;
    default:
        break;
    }

    postman->attach_to_server(args.hostname, args.port);

    state = S_START;
};

State Automata::set_state(State new_state)
{
    std::clog << "State changed from " << state << " to " << new_state << std::endl;
    state = new_state;
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
            state = s_start();
            break;
        case S_AUTH:
            state = s_auth();
            break;
        case S_OPEN:
            state = s_open();
            break;
        case S_ERROR:
            state = s_error();
            break;
        case S_END:
            state = s_end();
            return;
            break;
        default:
            state = s_error();
            break;
        }
    }

    return;
};

State Automata::s_start()
{
    // Poll for messages
    PollResult res = postman->poll_for_messages();

    // Handle the user message (ignore server messages)
    if (res.type == PollResultType::USER)
    {
        // Parse the message
        if (res.message.type == MessageType::AUTH)
        {
            AuthMessage auth = postman->data_to_auth(res.message.data);
            // Authorize the user
            postman->authorize(auth.username, display_name, auth.password);
            return S_AUTH;
        }
        else
        {
            // Error
            return S_ERROR;
        }
    }

    return state;
}

State Automata::s_auth()
{
    return S_END;
}

State Automata::s_open()
{
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
