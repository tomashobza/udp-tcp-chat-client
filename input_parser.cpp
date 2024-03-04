#include "input_parser.h"

std::vector<std::string> InputParser::tokenize_input(std::string input)
{
    std::vector<std::string> tokens;
    size_t pos = 0;
    std::string token;
    while ((pos = input.find_first_of(" \t\n\r\f\v")) != std::string::npos)
    {
        token = input.substr(0, pos);
        tokens.push_back(token);
        input.erase(0, pos + 1);
    }
    tokens.push_back(input);
    return tokens;
}

Command InputParser::get_command(std::string input)
{
    // Delete trailing whitespace at the beginning
    input = input.substr(input.find_first_not_of(" \t\n\r\f\v"));

    // Get the command
    std::string command = input.substr(0, input.find_first_of(" \t\n\r\f\v"));

    // Check if the command token is indeed a command
    if (command.at(0) == '/')
    {
        // Check if the command is a valid command
        if (command == "/auth")
        {
            return CMD_AUTH;
        }
        else if (command == "/join")
        {
            return CMD_JOIN;
        }
        else if (command == "/rename")
        {
            return CMD_RENAME;
        }
        else if (command == "/help")
        {
            return CMD_HELP;
        }
        else if (command == "/msg")
        {
            return CMD_MSG;
        }
        else
        {
            return CMD_ERROR;
        }
    }
    // Check if the command is a message
    else if (command.size() > 0)
    {
        return CMD_MSG;
    }
    // If the command is empty
    else
    {
        return CMD_ERROR;
    }

    return CMD_ERROR;
}