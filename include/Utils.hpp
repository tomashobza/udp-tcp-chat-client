#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <string>
#include <chrono>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include "types.hpp"

class Utils
{
public:
    /**
     * @brief Get the current timestamp.
     *
     * @return long long - miliseconds since epoch
     */
    static long long get_timestamp();

    /**
     * @brief Check if the stdin is closed.
     *
     * @return true
     * @return false
     */
    static bool is_stdin_closed();
};

#endif // UTILS_H