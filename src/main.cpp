#include "ArgParser.hpp"
#include "Automata.hpp"
#include <iostream>

int main(int argc, char *argv[])
{
    Args args = ArgParser::parse(argc, argv);

    Automata fsm(args);

    return fsm.run();
}