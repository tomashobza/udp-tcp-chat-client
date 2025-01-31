/**
 * @file InputParser.cpp
 * @author Tomáš Hobza (xhobza03)
 * @brief Input parser for the project
 * @date 2024-03-31
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "InputParser.hpp"

Command InputParser::parse_input()
{
    // Get the input from the user
    std::string input;
    std::getline(std::cin, input);

    // Parse the first word as the command
    size_t pos = input.find(' ');
    std::string command = input.substr(0, pos);

    // Parse the rest of the input as arguments
    std::vector<std::string> args;
    while (pos != std::string::npos)
    {
        size_t next_pos = input.find(' ', pos + 1);
        std::string arg = input.substr(pos + 1, next_pos - pos - 1);
        if (arg.size() > 0)
        {
            args.push_back(arg);
        }
        pos = next_pos;
    }

    // Determine the command type
    if (command == "/auth")
    {
        // Check if the number of arguments is correct
        if (args.size() != 3)
        {
            std::cerr << "ERR: wrong number of arguments" << std::endl;
            goto help;
        }
        else
        {
            return Command{CMD_AUTH, args};
        }
    }
    else if (command == "/join")
    {
        // Check if the number of arguments is correct
        if (args.size() != 1)
        {
            std::cerr << "ERR: wrong number of arguments" << std::endl;
            goto help;
        }
        else
        {
            return Command{CMD_JOIN, args};
        }
    }
    else if (command == "/rename")
    {
        // Check if the number of arguments is correct
        if (args.size() != 1)
        {
            std::cerr << "ERR: wrong number of arguments" << std::endl;
            goto help;
        }
        else
        {
            return Command{CMD_RENAME, args};
        }
    }
    else if (command == "/help")
    {
        goto help;
    }
    else if (command.at(0) == '/')
    {
        std::cerr << "ERR: not a valid command!" << std::endl;
        goto help;
    }
    else
    {
        // Check if the message is empty
        if (input.size() == 0)
        {
            std::cerr << "ERR: empty message not allowed" << std::endl;
            goto help;
        }
        // If the command is not recognized, assume it is a message
        return Command{CMD_MSG, {input}};
    }

help: // Help command

    // If the command is not recognized, print and return a help command
    std::cout << "\tAvailable commands:" << std::endl;
    std::cout << "\t/auth <username> <password> <display_name>" << std::endl;
    std::cout << "\t/join <channel_id>" << std::endl;
    std::cout << "\t/rename <new_display_name>" << std::endl;
    std::cout << "\t/help" << std::endl;

    return Command{CMD_HELP, {}};
}