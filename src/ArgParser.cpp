#include "ArgParser.h"

// TODO: Implement the parse function
Args ArgParser::parse(int argc, char *argv[])
{
    Args args = {Protocol::TCP, "localhost", 4567, 250, 3};

    bool has_hostname = false;
    bool has_type = false;

    int opt;
    while ((opt = getopt(argc, argv, "ht:s:p:d:r:")) != -1)
    {
        switch (opt)
        {
        case 'h':
            std::cout << "Usage: " << argv[0] << " [options]\n"
                      << "-t tcp|udp              Transport protocol used for connection\n"
                      << "-s IP address|hostname  Server IP or hostname\n"
                      << "-p 4567                 Server port (default: 4567)\n"
                      << "-d 250                  UDP confirmation timeout in milliseconds (default: 250)\n"
                      << "-r 3                    Maximum number of UDP retransmissions (default: 3)\n"
                      << "-h                      Print this help and exit\n";
            exit(EXIT_SUCCESS);
            break;

        case 't':
            if (strcmp(optarg, "tcp") == 0)
            {
                args.type = Protocol::TCP;
                has_type = true;
            }
            else if (strcmp(optarg, "udp") == 0)
            {
                args.type = Protocol::UDP;
                has_type = true;
            }
            else
            {
                throw std::invalid_argument("Invalid protocol type");
            }
            break;

        case 's':
            args.hostname = optarg;
            has_hostname = true;
            break;

        case 'p':
            args.port = atoi(optarg);
            break;

        case 'd':
            args.timeout = atoi(optarg);
            break;

        case 'r':
            args.max_tries = atoi(optarg);
            break;

        case '?':
        default:
            throw std::invalid_argument("Invalid option");
        }
    }

    // Check if the required arguments are present
    if (!has_hostname)
    {
        throw std::invalid_argument("No hostname provided");
    }

    if (!has_type)
    {
        throw std::invalid_argument("No protocol type provided");
    }

    return args;
}