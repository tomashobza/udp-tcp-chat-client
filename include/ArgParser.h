#ifndef ARG_PARSER_H
#define ARG_PARSER_H

#include "types.h"

class ArgParser
{
public:
    static Args parse(int argc, char *argv[]);
};

#endif // ARG_PARSER_H
