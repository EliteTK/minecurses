#include "minesweeper.h"
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>


struct ms_board *ms_newboard(unsigned width, unsigned height, unsigned mine_total)
{
	struct ms_board *board;

	board = malloc(sizeof(struct ms_board));
	board->grid = calloc(sizeof(struct ms_square), width * height);

	board->width = width;
	board->height = height;
	board->mines = mine_total;
	board->flags = 0;
	board->virgin = true;

	return board;
}

void ms_delboard(struct ms_board *board)
{
	int i;

	free(board->grid);
	free(board);
}

static unsigned rand_to_max(unsigned max)
{
	double random;

	while ((random = rand()) == (double)RAND_MAX);

	return random / (double)RAND_MAX * (double)max;
}

static unsigned char calc_value(struct ms_board *board, long x, long y)
{
	unsigned value;

	value = 0;
	for (int dx = -1; dx <= 1; dx++)
		for (int dy = -1; dy <= 1; dy++) {
			if ((x + dx) < 0 || (x + dx) >= board->width)
				continue;
			if ((y + dy) < 0 || (y + dy) >= board->height)
				continue;
			if (ms_resolve(board, x + dx, y + dy)->mine)
				value++;
		}

	return value;
}

void ms_genmap(struct ms_board *board, unsigned seed)
{
	unsigned mines_placed = 0;

	srand(seed);
	while (mines_placed <= board->mines) {
		unsigned x, y;
		x = rand_to_max(board->width);
		y = rand_to_max(board->height);
		if (ms_resolve(board, x, y)->mine)
			continue;
		ms_resolve(board, x, y)->mine = true;
		mines_placed++;
	}

	for (unsigned x = 0; x < board->width; x++)
		for (unsigned y = 0; y < board->height; y++)
			ms_resolve(board, x, y)->value = calc_value(board, x, y);
}

void ms_start(struct ms_board *board, unsigned startx, unsigned starty)
{
}

struct ms_square *ms_resolve(struct ms_board *board, unsigned x, unsigned y)
{
	return &board->grid[y * board->width + x];
}

unsigned char ms_getvalue(const Game *game, const unsigned int x, const unsigned int y)
{
	return ms_getsquare(game, x, y)->value;
}

static void reveal_spread(const Game *game, const unsigned int x, const unsigned int y)
{
	if (ms_getvisible(game, x, y))
		return;

	ms_setvisible(game, x, y, true);

	if (ms_getvalue(game, x, y))
		return;

	int dx, dy;
	for (dx = -1; dx <= 1; dx++)
		for (dy = -1; dy <= 1; dy++)
			if (!((dy == dx) && (dx == 0)) && WITHIN_BOUNDS(game, x + dx, y + dy))
				reveal_spread(game, x + dx, y + dy);
}

#undef WITHIN_BOUNDS

bool ms_reveal(const Game *game, const unsigned int x, const unsigned int y)
{
	if (ms_getmine(game, x, y))
		return false;

	reveal_spread(game, x, y);
	return true;
}

bool ms_reveal_aoe(const Game *game, const unsigned int x, const unsigned int y)
{
	unsigned char value;
	if (ms_getvisible(game, x, y) && (value = ms_getvalue(game, x, y))) {
		int dx, dy, flagtotal = 0;
		for (dx = -1; dx <= 1; dx++)
			for (dy = -1; dy <= 1; dy++)
				if (WITHIN_BOUNDS(game, x + dx, y + dy)
						&& !ms_getvisible(game, x + dx, y + dy)
						&& ms_getflag(game, x + dx, y + dy))
					flagtotal++;

		if (flagtotal != value)
			return true;

		bool failure = false;
		for (dx = -1; dx <= 1; dx++)
			for (dy = -1; dy <= 1; dy++)
				if (WITHIN_BOUNDS(game, x + dx, y + dy) && !ms_getflag(game, x + dx, y + dy))
					failure = failure || !ms_reveal(game, dx + x, dy + y);

		return !failure;
	}
	return true;
}

bool ms_check_won(const Game *game)
{
	if (game->mines != game->flags)
		return false;

	for (unsigned x = 0; x < game->sizex; x++)
		for (unsigned y = 0; y < game->sizey; y++)
			if (ms_getmine(game, x, y) != ms_getflag(game, x, y))
				return false;

	return true;
}
