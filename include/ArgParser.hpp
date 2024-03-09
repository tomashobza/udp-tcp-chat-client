#ifndef ARG_PARSER_H
#define ARG_PARSER_H

#include <unistd.h>
#include <iostream>
#include <string.h>
#include "types.hpp"

class ArgParser
{
public:
    static Args parse(int argc, char *argv[]);
};

#endif // ARG_PARSER_H
