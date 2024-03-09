#ifndef INPUT_PARSER_H
#define INPUT_PARSER_H

#include "types.hpp"
#include <iostream>
#include <string.h>

class InputParser
{
public:
    static Command parse_input();
};

#endif // INPUT_PARSER_H