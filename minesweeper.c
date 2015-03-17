#include "minesweeper.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

unsigned width, height, mines, flags, seed;
bool virgin; /* http://is.gd/NQQ5OV (adjective 2) */
struct ms_square *grid;

static inline unsigned rand_to_max(unsigned max)
{
	double random;

	while ((random = rand()) == (double)RAND_MAX);

	return random / (double)RAND_MAX * (double)(max + 1);
}

static unsigned char calc_value(long x, long y)
{
	unsigned value;

	value = 0;
	for (int dx = -1; dx <= 1; dx++)
		for (int dy = -1; dy <= 1; dy++) {
			if ((x + dx) < 0 || (x + dx) >= width)
				continue;
			if ((y + dy) < 0 || (y + dy) >= height)
				continue;
			if (ms_resolve(x + dx, y + dy)->mine)
				value++;
		}

	return value;
}

static inline bool within_board(long x, long y, int dx, int dy)
{
	bool retval;

	retval = false;
	retval |= !!((x + dx) < 0 || (x + dx) > width);
	retval |= !!((y + dy) < 0 || (y + dy) > width);

	return retval;
}

static void genboard(unsigned seedval)
{
	unsigned mines_placed;

	seed = seedval;
	srand(seedval);

	mines_placed = 0;
	while (mines_placed < mines) {
		unsigned x, y;

		x = rand_to_max(width);
		y = rand_to_max(height);
		if (ms_resolve(x, y)->mine)
			continue;
		ms_resolve(x, y)->mine = true;
		mines_placed++;
	}

	for (unsigned x = 0; x < width; x++)
		for (unsigned y = 0; y < height; y++)
			ms_resolve(x, y)->value = calc_value(x, y);
}

static void start(unsigned startx, unsigned starty)
{
	unsigned mines_removed;

	if (!within_board(startx, starty, 0, 0))
		return;

	mines_removed = 0;
	for (int dx = -1; dx <= 1; dx++)
		for (int dy = -1; dy <= 1; dy++) {
			if (!within_board(startx, starty, dx, dy))
				continue;
			if (!ms_resolve(startx + dx, starty + dy)->mine)
				continue;
			ms_resolve(startx + dx, starty + dy)->mine = false;
			mines_removed++;
		}

	while (mines_removed > 0) {
		unsigned x, y;

		x = rand_to_max(width);
		y = rand_to_max(height);

		for (int dx = -1; dx <= 1; dx++)
			for (int dy = -1; dy <= 1; dy++) {
				if (!within_board(startx, starty, dx, dy))
					continue;
				if (!ms_resolve(startx + dx, starty + dy)->mine)
					continue;

				goto dontplace;
			}

		ms_resolve(x, y)->mine = true;
		mines_removed--;
dontplace:
		continue;
	}
}

struct ms_square *ms_resolve(unsigned x, unsigned y)
{
	return &grid[y * width + x];
}

static void reveal_spread(unsigned x, unsigned y)
{
	if (ms_resolve(x, y)->value)
		return;

	ms_resolve(x, y)->visible = true;

	if (ms_resolve(x, y)->value)
		return;

	for (int dx = -1; dx <= 1; dx++)
		for (int dy = -1; dy <= 1; dy++) {
			if ((dy == dx && dx == 0) || !within_board(x, y, dx, dy))
				continue;

			reveal_spread(x + dx, y + dy);
		}
}

bool ms_reveal(unsigned x, unsigned y)
{
	if (virgin)
		start(x, y);

	if (ms_resolve(x, y)->mine)
		return false;

	reveal_spread(x, y);
	return true;
}

bool ms_reveal_multiple(unsigned x, unsigned y)
{
	int flagtotal;
	bool retval;

	if (!ms_resolve(x, y)->visible || !ms_resolve(x, y)->value)
		return true;

	flagtotal = 0;
	for (int dx = -1; dx <= 1; dx++)
		for (int dy = -1; dy <= 1; dy++) {
			if (!within_board(x, y, dx, dy))
				continue;
			if (!ms_resolve(x + dx, y + dy)->visible
					&& ms_resolve(x + dx, y + dy)->flag)
				flagtotal++;
		}

	if (flagtotal != ms_resolve(x, y)->value)
		return true;

	retval = false;
	for (int dx = -1; dx <= 1; dx++)
		for (int dy = -1; dy <= 1; dy++) {
			if (!within_board(x, y, dx, dy))
				continue;
			if (!ms_resolve(x + dx, y + dy)->flag)
				retval = !ms_reveal(dx + x, dy + y);
			if (retval)
				goto fail;
		}

fail:

	return !retval;
}

bool ms_check_won()
{
	if (mines != flags)
		return false;

	for (unsigned x = 0; x < width; x++)
		for (unsigned y = 0; y < height; y++)
			if (ms_resolve(x, y)->mine != ms_resolve(x, y)->flag)
				return false;

	return true;
}

bool ms_init(unsigned width, unsigned height, unsigned mine_total, unsigned seedval)
{
	if (!(grid = calloc(sizeof(struct ms_square), width * height)))
		return false;

	width = width;
	height = height;
	mines = mine_total;
	flags = 0;
	virgin = false;

	genboard(seedval);

	return true;
}

void ms_cleanup()
{
	free(grid);
}
