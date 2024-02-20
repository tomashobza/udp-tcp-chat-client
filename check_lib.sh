#!/bin/bash

# Define a list of allowed non-standard libraries and categories
additionalAllowedLibraries=(
    "arpa/inet.h" # Example of allowed non-standard header files
    "netdb.h"
    "sys/socket.h"
    "netinet/in.h"
    "libpcap" # Specific allowed libraries
    "libnet"
    "sys/types"
    "sys/uio"
    "errno"
    "sys/ioctl"
    "fcntl"
    "sys/socket"
    "netdb"
    "netinet/in_systm"
    "netinet/ip_icmp"
    "netinet/udp"
    "netinet/ip"
    "netinet/in"
    "arpa/inet"
    "arpa/nameser"
    "resolv"
    "net/if"
    "strings"
    "unistd"
    "sys/_endian"
    # Add any additional non-standard allowed libraries or headers here
)

# Define all standard libraries provided
standardLibraries=(
    "assert.h"
    "complex.h"
    "ctype.h"
    "errno.h"
    "fenv.h"
    "float.h"
    "inttypes.h"
    "iso646.h"
    "limits.h"
    "locale.h"
    "math.h"
    "setjmp.h"
    "signal.h"
    "stdalign.h"
    "stdarg.h"
    "stdatomic.h"
    "stdbit.h"
    "stdbool.h"
    "stdckdint.h"
    "stddef.h"
    "stdint.h"
    "stdio.h"
    "stdlib.h"
    "stdnoreturn.h"
    "string.h"
    "tgmath.h"
    "threads.h"
    "time.h"
    "uchar.h"
    "wchar.h"
    "wctype.h"
    # Add more standard libraries here if needed
)

# Combine standard and additional allowed libraries into one array
allowedLibraries=("${standardLibraries[@]}" "${additionalAllowedLibraries[@]}")

# Convert allowed libraries to a grep pattern
allowedPattern=$(printf "|%s" "${allowedLibraries[@]}")
allowedPattern="${allowedPattern:1}" # Remove the leading '|'

# Find, sort, and count unique includes, then check if they are allowed or not
find . -type f \( -name "*.c" -o -name "*.h" \) -exec grep -hEo '#include <[^>]+>' {} + |
    sed 's/#include <\([^>]*\)>/\1/' |
    sort | uniq -c | sort -k1,1nr -k2 |
    while read -r count lib; do
        if [[ "$lib" =~ $allowedPattern ]]; then
            echo "$count $lib"
        else
            echo "$count $lib (not allowed)"
        fi
    done

# Notes:
# - This script now checks against a combined list of standard and specified additional allowed libraries.
# - Libraries not matching any in the combined allowed list are marked as "(not allowed)".
