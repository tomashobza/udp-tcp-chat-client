/**
 * @file ArgParser.hpp
 * @author Tomáš Hobza (xhobza03)
 * @brief Header file for the ArgParser class
 * @date 2024-03-31
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef ARG_PARSER_H
#define ARG_PARSER_H

#include <unistd.h>
#include <iostream>
#include <string.h>
#include "types.hpp"

/**
 * @brief Class to parse the command line arguments
 */
class ArgParser
{
public:
    /**
     * @brief Parse the command line arguments
     *
     * @param argc Number of arguments
     * @param argv Array of arguments
     * @return Args - The parsed arguments
     */
    static Args parse(int argc, char *argv[]);
};

#endif // ARG_PARSER_H
