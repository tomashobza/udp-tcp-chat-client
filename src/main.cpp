#include "ArgParser.h"
#include <iostream>

int main(int argc, char *argv[])
{
    Args args = ArgParser::parse(argc, argv);

    return 0;
}