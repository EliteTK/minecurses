#include "minesweeper.h"
#include <stdbool.h>
#include <stdint.h>
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

static inline unsigned rand_to_max(unsigned max)
{
	double random;

	while ((random = rand()) == (double)RAND_MAX);

	return random / (double)RAND_MAX * (double)(max + 1);
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

static inline bool within_board(struct ms_board *board, unsigned x, unsigned y, int dx, int dy)
{
	bool retval;

	retval |= !!((x + dx) < 0 || (x + dx) > board->width);
	retval |= !!((y + dy) < 0 || (y + dy) > board->width);

	return retval;
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

static void reveal_spread(struct ms_board *board, unsigned x, unsigned y)
{
	if (ms_resolve(board, x, y)->value)
		return;

	ms_resolve(board, x, y)->visible = true;

	if (ms_resolve(board, x, y)->value)
		return;

	for (int dx = -1; dx <= 1; dx++)
		for (int dy = -1; dy <= 1; dy++) {
			if ((dy == dx == 0) || !within_board(board, x, y, dx, dy))
				continue;

			reveal_spread(board, x + dx, y + dy);
		}
}

bool ms_reveal(struct ms_board *board, unsigned x, unsigned y)
{
	if (ms_resolve(board, x, y)->mine)
		return false;

	reveal_spread(board, x, y);
	return true;
}

bool ms_reveal_aoe(struct ms_board *board, unsigned x, unsigned y)
{
	int flagtotal;
	bool retval;

	if (!ms_resolve(board, x, y)->visible
			|| !ms_resolve(board, x, y)->value)
		return true;

	flagtotal = 0;
	for (int dx = -1; dx <= 1; dx++)
		for (int dy = -1; dy <= 1; dy++) {
			if (!within_board(board, x, y, dx, dy))
				continue;
			if (!ms_resolve(board, x + dx, y + dy)->visible
					&& ms_resolve(board, x + dx, y + dy)->flag)
				flagtotal++;
		}

	if (flagtotal != ms_resolve(board, x, y)->value)
		return true;

	retval = false;
	for (int dx = -1; dx <= 1; dx++)
		for (int dy = -1; dy <= 1; dy++) {
			if (!within_board(board, x, y, dx, dy))
				continue;
			if (!ms_resolve(board, x + dx, y + dy)->flag)
				retval = !ms_reveal(board, dx + x, dy + y);
			if (retval)
				goto fail;
		}

fail:

	return !retval;
}

bool ms_check_won(struct ms_board *board)
{
	if (board->mines != board->flags)
		return false;

	for (unsigned x = 0; x < board->width; x++)
		for (unsigned y = 0; y < board->height; y++)
			if (ms_resolve(board, x, y)->mine != ms_resolve(board, x, y)->flag)
				return false;

	return true;
}
