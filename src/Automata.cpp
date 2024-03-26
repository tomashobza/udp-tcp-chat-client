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
    // Set allowed inputs from the user
    postman->allow_client_commands({CommandType::CMD_AUTH});

    // Poll for messages
    PollResults results = postman->poll_for_messages();

    for (auto &res : results)
    {
        // Handle the user message
        if (res.type == PollResultType::USER)
        {
            // Parse the message
            switch (res.message.type)
            {
            case MessageType::AUTH:
                // Send the message
                postman->authorize(res.message.username, res.message.display_name, res.message.password);
                set_state(S_AUTH);
                break;
            case MessageType::BYE:
                postman->bye();
                set_state(S_END);
                break;
            default:
                std::cerr << "ERR: Unexpected user command!" << std::endl;
                postman->error(display_name, "Unexpected user command!");
                set_state(S_ERROR);
                break;
            }
        }
        // Handle the server message
        else if (res.type == PollResultType::SERVER)
        {
            switch (res.message.type)
            {
            case MessageType::ERR:
                std::cerr << "ERR FROM " << res.message.display_name << ": " << res.message.contents << std::endl;
                set_state(S_ERROR);
                break;
            default:
                std::cerr << "ERR: Unexpected message from server!" << std::endl;
                postman->error(display_name, "Unexpected message from server!");
                set_state(S_ERROR);
                break;
            }
        }
    }

    return state;
}

State Automata::s_auth()
{
    // Set allowed inputs from the user and server
    postman->allow_client_commands({CommandType::CMD_AUTH});

    // Poll for messages
    PollResults results = postman->poll_for_messages();

    for (auto &res : results)
    {
        if (res.type == PollResultType::USER)
        {
            switch (res.message.type)
            {
            case MessageType::AUTH:
                // Send the message
                postman->authorize(res.message.username, res.message.display_name, res.message.password);
                break;
            case MessageType::BYE:
                postman->bye();
                set_state(S_END);
                break;
            default:
                std::cerr << "ERR: Unexpected user command!" << std::endl;
                postman->error(display_name, "Unexpected user command!");
                set_state(S_ERROR);
                break;
            }
        }
        if (res.type == PollResultType::SERVER)
        {
            switch (res.message.type)
            {
            case MessageType::REPLY:
                if (res.message.type == MessageType::REPLY && res.message.result == 1)
                {
                    std::cerr << "Success: " << res.message.contents << std::endl;
                    set_state(S_OPEN);
                }
                else
                {
                    std::cerr << "Failure: " << res.message.contents << std::endl;
                }
                break;
            case MessageType::ERR:
                std::cerr << "ERR FROM " << res.message.display_name << ": " << res.message.contents << std::endl;
                postman->bye();
                set_state(S_END);
                break;
            default:
                std::cerr << "ERR: Unexpected message from server!" << std::endl;
                postman->error(display_name, "Unexpected message from server!");
                set_state(S_ERROR);
                break;
            }
        }
    }

    return state;
}

State Automata::s_open()
{
    // Set allowed inputs from the user and server
    postman->allow_client_commands({CommandType::CMD_MSG, CommandType::CMD_JOIN});

    // Poll for messages
    PollResults results = postman->poll_for_messages();

    for (auto &res : results)
    {
        if (res.type == PollResultType::USER)
        {
            switch (res.message.type)
            {
            case MessageType::JOIN:
                // Send the message
                postman->join(res.message.channel_id, res.message.display_name);
                break;

            case MessageType::MSG:
                // Send the message
                postman->message(res.message.display_name, res.message.contents);
                break;

            case MessageType::BYE:
                postman->bye();
                set_state(S_END);
                break;

            default:
                std::cerr << "Unexpected user command!" << std::endl;
                set_state(S_ERROR);
                break;
            }
        }
        else if (res.type == PollResultType::SERVER)
        {
            switch (res.message.type)
            {
            case MessageType::REPLY:
                if (res.message.type == MessageType::REPLY && res.message.result == 1)
                {
                    std::cerr << "Success: " << res.message.contents << std::endl;
                }
                else
                {
                    std::cerr << "Failure: " << res.message.contents << std::endl;
                }
                break;
            case MessageType::MSG:
                std::cout << res.message.display_name << ": " << res.message.contents << std::endl;
                break;
            case MessageType::ERR:
                std::cerr << "ERR FROM " << res.message.display_name << ": " << res.message.contents << std::endl;
                postman->bye();
                set_state(S_END);
                break;
            case MessageType::BYE:
                set_state(S_END);
                break;
            default:
                std::cerr << "ERR: Unexpected message from server!" << std::endl;
                postman->error(display_name, "Unexpected message from server!");
                set_state(S_ERROR);
                break;
            }
        }
    }

    return state;
}

State Automata::s_error()
{
    // Set allowed inputs from the user and server
    postman->allow_client_commands({});

    // Poll for messages
    PollResults results = postman->poll_for_messages();

    for (auto &res : results)
    {
        if (res.type == PollResultType::USER)
        {
            switch (res.message.type)
            {
            case MessageType::BYE:
                postman->bye();
                set_state(S_END);
                break;
            default:
                std::cerr << "ERR: Unexpected user command!" << std::endl;
                postman->error(display_name, "Unexpected user command!");
                set_state(S_ERROR);
                break;
            }
        }
        else if (res.type == PollResultType::SERVER)
        {
            switch (res.message.type)
            {
            case MessageType::ERR:
                std::cerr << "ERR FROM " << res.message.display_name << ": " << res.message.contents << std::endl;
                postman->bye();
                set_state(S_END);
                break;
            default:
                std::cerr << "ERR: Unexpected message from server!" << std::endl;
                postman->error(display_name, "Unexpected message from server!");
                set_state(S_ERROR);
                break;
            }
        }
    }

    return state;
}

State Automata::s_end()
{
    std::cout << "Goodbye!" << std::endl;
    return S_END;
}
