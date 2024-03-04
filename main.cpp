#include <iostream>
#include <sys/select.h>

#include "fsm.h"
#include "postman.h"
// #include "confirm_worker.h"

int main()
{
    Automata fsm;

    fsm.start_yaaping();

    exit(EXIT_SUCCESS);
}
