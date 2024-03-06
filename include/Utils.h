#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <string>
#include "types.h"

class Utils
{
    /**
     * @brief Read the stdin and return the input.
     *
     * @return std::string
     */
    static std::string read_stdin();
};

#endif // UTILS_H