#ifndef SOCKS_H
#define SOCKS_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <errno.h>
#include <stdexcept>
#include <vector>
#include <sys/time.h>

/**
 * @brief A class for creating and managing sockets.
 */
class Sock
{
private:
    /** Socket file descriptor */
    int fd;
    /** Is the socket in blocking mode */
    bool is_blocking = true;

public:
    /**
     * @brief Construct a new Socket object and create a socket.
     *
     */
    Sock();
    ~Sock();

    /**
     * @brief Set the socket file descriptor to the non-blocking mode.
     *
     * @return bool - is socket blocking
     */
    bool setSocketNonBlocking();

    /**
     * @brief Set the socket file descriptor to the blocking mode.
     *
     * @return bool - is socket blocking
     */
    bool setSocketBlocking();

    /**
     * @brief Get the socket file descriptor.
     *
     * @return bool - is socket blocking
     */
    bool isBlocking();

    /**
     * @brief Get the socket file descriptor.
     *
     * @return int
     */
    int getFd();
};

#endif // SOCKS_H