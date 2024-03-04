#ifndef INPUT_PARSER_H
#define INPUT_PARSER_H

#include <string>
#include <iostream>
#include <vector>
#include "types.h"

typedef enum
{
    CMD_AUTH,
    CMD_JOIN,
    CMD_RENAME,
    CMD_HELP,
    CMD_MSG,
    CMD_ERROR,
} Command;

class InputParser
{
public:
    /**
     * @brief Tokenize the input string.
     *
     * @param input
     * @return std::vector<std::string> array of tokens
     */
    static std::vector<std::string> tokenize_input(std::string input);

    /**
     * @brief Get the command from a string.
     *
     * @param input
     * @return Command
     */
    static Command get_command(std::string input);
};

#endif // INPUT_PARSER_H