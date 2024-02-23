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
    Postman postman;
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
     * @brief Open polling for the automata for the client socket and stdin.
     */
    void open_polling();
};

#endif // AUTOMATA_H