/**
 * @file Utils.cpp
 * @author Tomáš Hobza (xhobza03)
 * @brief Utility functions for the project
 * @date 2024-03-31
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "Utils.hpp"

long long Utils::get_timestamp()
{
    // Get the current time since epoch in milliseconds
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

bool Utils::is_stdin_closed()
{
    // Prepare the pollfd structure
    struct pollfd fds;
    fds.fd = 0;          // stdin
    fds.events = POLLIN; // Check for normal or out-of-band data
    fds.revents = 0;

    // Use poll to check if stdin is ready for reading without blocking
    int ret = poll(&fds, 1, 0); // Timeout is set to 0 for non-blocking

    if (ret == -1)
    {
        // An error occurred
        return true; // Assume stdin is closed on error
    }
    else if (ret == 0)
    {
        // No data available to read - stdin is not necessarily closed
        return false;
    }
    else
    {
        if (fds.revents & POLLIN)
        {
            // Data is available to read. Now, check if it can be read or if it's EOF
            char buffer;
            ssize_t bytesRead = read(0, &buffer, 1);

            if (bytesRead <= 0)
            {
                // If EOF or error, assume stdin is closed
                return true;
            }

            // If data was read successfully, push it back onto stdin and indicate it's open
            ungetc(buffer, stdin);
            return false;
        }
        else if (fds.revents & (POLLHUP | POLLERR | POLLNVAL))
        {
            // If poll reports hang-up, error, or invalid request, assume stdin is closed
            return true;
        }
    }

    // Default to not closed if none of the conditions above are met
    return false;
}
