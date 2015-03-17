#ifndef __MINESWEEPER_H
#include <stdbool.h>

struct ms_square{
	unsigned char query : 1, flag : 1, visible : 1, mine : 1, value : 4;
};

bool ms_init(unsigned width, unsigned height, unsigned mine_total, unsigned seedval);
void ms_cleanup();
struct ms_square *ms_resolve(unsigned x, unsigned y);
bool ms_reveal(unsigned x, unsigned y);
bool ms_reveal_multiple(unsigned x, unsigned y);
bool ms_check_won();

#define __MINESWEEPER_H
#endif
