// Can't live without io and memory allocation
#include <stdio.h>
#include <stdlib.h>
// Not necessary but recomended for portability in some systems
#include <sys/types.h>
// The socket lib
#include <sys/socket.h>
// For closing
#include <unistd.h>
// We are going to need this to work with the addresses
#include <string.h>
// This is where getnameinfo is
#include <netdb.h>
#include <sys/_endian.h>

int main(void)
{
    printf("Hello, World!\n");
    return 0;
}