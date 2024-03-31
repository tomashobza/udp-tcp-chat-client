/**
 * @file Utils.hpp
 * @author Tomáš Hobza (xhobza03)
 * @brief Utils class for the project
 * @date 2024-03-31
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <string>
#include <chrono>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include "types.hpp"

/**
 * @brief Utils class
 *
 */
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