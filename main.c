#include <stdio.h>
#include "types.h"
#include <time.h>
#include <stdlib.h>

#define MONOPOLY_IMPLEMENTATION

#include "board.c"
#include "players.c"
#include "finance.c"
#include "events.c"
#include "game.c"

int main(void)
{
    srand((unsigned int)time(NULL));

    initialize_game();
    run_game();

    return 0;
}
