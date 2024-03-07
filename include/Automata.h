#ifndef AUTOMATA_H
#define AUTOMATA_H

#include "types.h"
#include "postman.h"
#include "UdpPostman.h"
#include "TcpPostman.h"

class Automata
{
private:
    IPostman *postman;
    State state = S_START;
    std::string display_name = "User";

public:
    Automata(Args args);
    ~Automata();

    /**
     * @brief Set the state of the automata.
     *
     * @param new_state - the new state
     * @return State - the new state
     */
    State set_state(State new_state);

    /**
     * @brief Change the display name of the user.
     *
     * @param new_display_name
     */
    void rename(const std::string &new_display_name);

    // STATE METHODS

    /**
     * @brief Start state.
     *
     * @return State - the next state
     */
    State s_start();

    /**
     * @brief Auth state.
     *
     * @return State - the next state
     */
    State s_auth();

    /**
     * @brief Open state.
     *
     * @return State - the next state
     */
    State s_open();

    /**
     * @brief Error state.
     *
     * @return State - the next state
     */
    State s_error();

    /**
     * @brief End state.
     *
     * @return State - the next state
     */
    State s_end();

    /**
     * @brief Run the FSM.
     */
    void run();
};

#endif // AUTOMATA_H