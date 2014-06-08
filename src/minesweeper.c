#include "minesweeper.h"
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define WITHIN_BOUNDS(g, x, y) ((0 <= (x) && (x) < (g->sizex)) && (0 <= (y) && (y) < (g->sizey)))

static unsigned char calc_value(const Game *, const unsigned int, const unsigned int);
inline Square *ms_xy_resolve(const Game *, const unsigned int, const unsigned int);

// Make a new game (You just lost the game.).
Game *ms_newgame(const unsigned int sizex, const unsigned int sizey, const unsigned int mine_total)
{
    Game *game = malloc(sizeof(Game));
    game->grid = calloc(sizeof(Square), sizex*sizey);
    game->sizex = sizex;
    game->sizey = sizey;
    game->mines = mine_total;
    game->flags = 0;
    game->generated = false;
    return game;
}

// Delete the game (You lost the game again.).
void ms_delgame(Game *game)
{
    free(game->grid);
    free(game);
}

// Clear the map, randomize the map, and populate the valuemap.
void ms_genmap(Game *game, const unsigned int startx, const unsigned int starty)
{
    int x, y;
    for (x = 0; x < game->sizex; x++)
        for (y = 0; y < game->sizey; y++) {
            ms_xy_resolve(game, x, y)->mine = false;
            ms_xy_resolve(game, x, y)->visible = false;
            ms_xy_resolve(game, x, y)->flag = false;
            ms_xy_resolve(game, x, y)->query = false;
            ms_xy_resolve(game, x, y)->value = 0;
        }


    srand(time(NULL));
    int mines_placed = 0;
    while (mines_placed <= game->mines) {
        int x = (int)((double)rand() / ((double)RAND_MAX + 1.0) * (double)game->sizex);
        int y = (int)((double)rand() / ((double)RAND_MAX + 1.0) * (double)game->sizey);
        if (ms_xy_resolve(game, x, y)->mine
                || (((signed int)(startx - 1) <= x && x <= startx + 1)
                    && ((signed int)(starty - 1) <= y && y <= starty + 1)))
            continue;
        ms_xy_resolve(game, x, y)->mine = true;
        mines_placed++;
    }


    for (x = 0; x < game->sizex; x++)
        for (y = 0; y < game->sizey; y++)
            ms_xy_resolve(game, x, y)->value = calc_value(game, x, y);

    game->generated = true;

    ms_reveal(game, startx, starty);
}

// Calculate total value of mines in 3 * 3 area around (x, y).
static unsigned char calc_value(const Game *game, const unsigned int x, const unsigned int y)
{
    int dx, dy, value = 0;
    for (dx = -1; dx <= 1; dx++)
        for (dy = -1; dy <= 1; dy++)
            if (WITHIN_BOUNDS(game, x + dx, y + dy) && ms_xy_resolve(game, x + dx, y + dy)->mine)
                value++;
    return value;
}

// Get the square corresponding to the coordinates.
Square *ms_xy_resolve(const Game *game, const unsigned int x, const unsigned int y)
{
    return &(game->grid[y * game->sizex + x]);
}

////////////////////////
// Main Functionality //
////////////////////////
static void reveal_spread(const Game *, const unsigned int, const unsigned int);

// Reveal a square, and in certain cases, spread like rabbits.
bool ms_reveal(const Game *game, const unsigned int x, const unsigned int y)
{
    if (ms_xy_resolve(game, x, y)->mine)
        return false;

    reveal_spread(game, x, y);
    return true;
}

// The actual working end of the function above.
static void reveal_spread(const Game *game, const unsigned int x, const unsigned int y)
{
    if (ms_xy_resolve(game, x, y)->visible)
        return;

    ms_xy_resolve(game, x, y)->visible = true;

    if (ms_xy_resolve(game, x, y)->value)
        return;

    int dx, dy;
    for (dx = -1; dx <= 1; dx++)
        for (dy = -1; dy <= 1; dy++)
            if (!((dy == dx) && (dx == 0)) && WITHIN_BOUNDS(game, x + dx, y + dy))
                reveal_spread(game, x + dx, y + dy);
}

// Area of effect reveal, if you know what middle click did in old minesweeper,
// you will certainly understand what this does.
bool ms_reveal_aoe(const Game *game, const unsigned int x, const unsigned int y)
{
    unsigned char value;
    if (ms_xy_resolve(game, x, y)->visible && (value = ms_xy_resolve(game, x, y)->value)) {
        int dx, dy, flagtotal = 0;
        for (dx = -1; dx <= 1; dx++)
            for (dy = -1; dy <= 1; dy++)
                if (WITHIN_BOUNDS(game, x + dx, y + dy)
                        && !ms_xy_resolve(game, x + dx, y + dy)->visible
                        && ms_xy_resolve(game, x + dx, y + dy)->flag)
                    flagtotal++;

        if (flagtotal != value)
            return true;

        bool failure = false;
        for (dx = -1; dx <= 1; dx++)
            for (dy = -1; dy <= 1; dy++)
                if (WITHIN_BOUNDS(game, x + dx, y + dy) && !ms_xy_resolve(game, x + dx, y + dy)->flag)
                    failure = failure || !ms_reveal(game, dx + x, dy + y);

        return !failure;
    }
    return true;
}

// Toggle a flag, and keep track of the number of flags.
void ms_flag_toggle(Game *game, const unsigned int x, const unsigned int y)
{
    Square *square = ms_xy_resolve(game, x, y);
    game->flags = square->flag ? game->flags - 1 : game->flags + 1;
    square->flag = !square->flag;
}


// Check if we've won the game yet.
bool ms_check_win(const Game *game)
{
    if (game->mines != game->flags)
        return false;

    int x, y;
    for (x = 0; x < game->sizex; x++)
        for (y = 0; y < game->sizey; y++)
            if (ms_xy_resolve(game, x, y)->mine != ms_xy_resolve(game, x, y)->flag)
                return false;

    return true;
}
