/**
 * @file main.cpp
 * @author Tomáš Hobza (xhobza03)
 * @brief Main file for the project
 * @date 2024-03-31
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "ArgParser.hpp"
#include "Automata.hpp"
#include <iostream>

int main(int argc, char *argv[])
{
    // Parse the CLI arguments
    Args args = ArgParser::parse(argc, argv);

    // Create the finite state machine
    Automata fsm(args);

    // Run the finite state machine and return the exit code to the OS
    return fsm.run();
}