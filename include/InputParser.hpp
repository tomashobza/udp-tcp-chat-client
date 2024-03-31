/**
 * @file InputParser.hpp
 * @author Tomáš Hobza (xhobza03)
 * @brief Input parser for the project
 * @date 2024-03-31
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef INPUT_PARSER_H
#define INPUT_PARSER_H

#include "types.hpp"
#include <iostream>
#include <string.h>

/**
 * @brief Input parser class
 */
class InputParser
{
public:
    /**
     * @brief Parse input from stdin
     *
     * @return Command - parsed command from stdin
     */
    static Command parse_input();
};

#endif // INPUT_PARSER_H