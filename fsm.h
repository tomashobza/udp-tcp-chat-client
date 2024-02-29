#ifndef AUTOMATA_H
#define AUTOMATA_H

#include "postman.h"
#include <sys/event.h>

typedef enum
{
    S_START,
    S_AUTH,
    S_OPEN,
    S_ERROR,
    S_END
} State;

class Automata
{
    IPostman *postman;
    State state = S_START;

public:
    Automata();
    ~Automata();

    /**
     * @brief Read the stdin and return the input.
     *
     * @return std::string
     */
    std::string read_stdin();

    /**
     * @brief Handle the message received from the server.
     *
     * @param msg The message received from the server.
     */
    void handle_msg(Message msg);

    /**
     * @brief Open polling for the automata for the client socket and stdin.
     *
     * @return State The next state of the automata.
     */
    State open_polling();

    /**
     * @brief Run the automata.
     */
    void run();

    static void print_leader();
};

#endif // AUTOMATA_H