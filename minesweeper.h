#ifndef __MINESWEEPER_H
#include <stdbool.h>

struct ms_square{
    unsigned char query : 1, flag : 1, visible : 1, mine : 1, value : 4;
};

struct ms_board {
    unsigned int width, height, mines, flags;
    bool virgin; /* http://is.gd/NQQ5OV (adjective 2) */
    struct ms_square *grid;
};

struct ms_board *ms_newgame(unsigned, unsigned, unsigned);
void ms_delgame(struct ms_board *);
void ms_genmap(struct ms_board *, unsigned);
void ms_start(struct ms_board *, unsigned, unsigned);
struct ms_square *ms_resolve(struct ms_board *, unsigned, unsigned);
bool ms_reveal(struct ms_board *, unsigned, unsigned);
bool ms_reveal_multiple(struct ms_board *, unsigned, unsigned);
bool ms_check_won(struct ms_board *);

#define __MINESWEEPER_H
#endif
