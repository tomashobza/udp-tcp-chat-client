/**
 * @file Automata.hpp
 * @author Tomáš Hobza (xhobza03)
 * @brief Header file for the Automata class
 * @date 2024-03-31
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef AUTOMATA_H
#define AUTOMATA_H

#include "types.hpp"
#include "Postman.hpp"
#include "UdpPostman.hpp"
#include "TcpPostman.hpp"

/**
 * @brief The Automata class
 */
class Automata
{
private:
    /** The postman object */
    IPostman *postman;
    /** The current state of the automata */
    State state = S_START;
    /** Return code */
    int ret_code = EXIT_SUCCESS;

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
     *
     * @return int - the return code
     */
    int run();
};

#endif // AUTOMATA_H