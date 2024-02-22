#include "sock.h"

Sock::Sock()
{
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0)
    {
        throw std::runtime_error("ERROR opening socket");
    }
    std::clog << "Created client socket: " << fd << std::endl;
}

Sock::~Sock()
{
    close(fd);
    std::clog << "Closed client socket: " << fd << std::endl;
}

bool Sock::setSocketNonBlocking()
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
        return false;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK) != -1;
}
bool Sock::setSocketBlocking()
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
        return false;
    return fcntl(fd, F_SETFL, flags & ~O_NONBLOCK) != -1;
}
bool Sock::isBlocking()
{
    return is_blocking;
}

int Sock::getFd()
{
    return fd;
}

void Sock::set_timeout(int msec)
{
    // set timeout for recvfrom
    const struct timeval tv
    {
        .tv_sec = msec / 1000,
        .tv_usec = (msec * 1000) % 1000000
    };
    if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
    {
        throw std::runtime_error("ERROR setting socket options");
    }
}

void Sock::unset_timeout()
{
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, NULL, 0);
}