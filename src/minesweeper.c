#include "minesweeper.h"
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define WITHIN_BOUNDS(g, x, y) ((0 <= (x) && (x) < (g->sizex)) && (0 <= (y) && (y) < (g->sizey)))

static unsigned char calc_value(const Game *, const unsigned int, const unsigned int);

// Make a new game (You just lost the game.).
Game *ms_newgame(const unsigned int sizex, const unsigned int sizey, const unsigned int mine_total)
{
    Game *game = malloc(sizeof(Game));
    game->grid = malloc(sizeof(Square *)*sizex*sizey);
    int i;
    for (i = 0; i < sizex * sizey; i++)
        game->grid[i] = malloc(sizeof(Square));
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
    int i;
    for (i = 0; i < game->sizex * game->sizey; i++)
        free(game->grid[i]);
    free(game->grid);
    free(game);
}

// Clear the map, randomize the map, and populate the valuemap.
void ms_genmap(Game *game, const unsigned int startx, const unsigned int starty)
{
    int x, y;
    for (x = 0; x < game->sizex; x++)
        for (y = 0; y < game->sizey; y++) {
            ms_setmine(game, x, y, false);
            ms_setvisible(game, x, y, false);
            ms_setflag(game, x, y, false);
            ms_setquery(game, x, y, false);
            ms_setvisible(game, x, y, false);
        }


    srand(time(NULL));
    int mines_placed = 0;
    while (mines_placed <= game->mines) {
        int x = (int)((double)rand() / ((double)RAND_MAX + 1.0) * (double)game->sizex);
        int y = (int)((double)rand() / ((double)RAND_MAX + 1.0) * (double)game->sizey);
        if (ms_getmine(game, x, y) || (((signed int)(startx - 1) <= x && x <= startx + 1)
                                   && ((signed int)(starty - 1) <= y && y <= starty + 1)))
            continue;
        ms_setmine(game, x, y, true);
        mines_placed++;
    }


    for (x = 0; x < game->sizex; x++)
        for (y = 0; y < game->sizey; y++)
            ms_setvalue(game, x, y, calc_value(game, x, y));

    game->generated = true;

    ms_reveal(game, startx, starty);
}

// Calculate total value of mines in 3 * 3 area around (x, y).
static unsigned char calc_value(const Game *game, const unsigned int x, const unsigned int y)
{
    int dx, dy, value = 0;
    for (dx = -1; dx <= 1; dx++)
        for (dy = -1; dy <= 1; dy++)
            if (WITHIN_BOUNDS(game, x + dx, y + dy) && ms_getmine(game, x + dx, y + dy))
                value++;
    return value;
}

// Class-esque stuff, will be going out of the windows, I have no idea what I
// was thinking when I first implemented this. Totally worthless.

/////////////
// Getters //
/////////////

Square *ms_getsquare(const Game *game, const unsigned int x, const unsigned int y)
{
    return game->grid[y * game->sizex + x];
}

bool ms_getmine(const Game *game, const unsigned int x, const unsigned int y)
{
    return ms_getsquare(game, x, y)->mine;
}

bool ms_getvisible(const Game *game, const unsigned int x, const unsigned int y)
{
    return ms_getsquare(game, x, y)->visible;
}

bool ms_getflag(const Game *game, const unsigned int x, const unsigned int y)
{
    return ms_getsquare(game, x, y)->flag;
}

bool ms_getquery(const Game *game, const unsigned int x, const unsigned int y)
{
    return ms_getsquare(game, x, y)->query;
}

unsigned char ms_getvalue(const Game *game, const unsigned int x, const unsigned int y)
{
    return ms_getsquare(game, x, y)->value;
}

/////////////
// Setters //
/////////////

void ms_setsquare(const Game *game, const unsigned int x, const unsigned int y, Square *square)
{
    game->grid[y * game->sizex + x] = square;
}

void ms_setmine(const Game *game, const unsigned int x, const unsigned int y, const bool mine)
{
    ms_getsquare(game, x, y)->mine = mine;
}

void ms_setvisible(const Game *game, const unsigned int x, const unsigned int y, const bool visible)
{
    ms_getsquare(game, x, y)->visible = visible;
}

void ms_setflag(Game *game, const unsigned int x, const unsigned int y, const bool flag)
{
    Square *square = ms_getsquare(game, x, y);
    if (square->flag != flag)
        game->flags = flag ? game->flags + 1 : game->flags - 1;
    square->flag = flag;
}

void ms_setquery(const Game *game, const unsigned int x, const unsigned int y, const bool query)
{
    ms_getsquare(game, x, y)->query = query;
}

void ms_setvalue(const Game *game, const unsigned int x, const unsigned int y, const unsigned char value)
{
    ms_getsquare(game, x, y)->value = value;
}

////////////////////////
// Main Functionality //
////////////////////////
static void reveal_spread(const Game *, const unsigned int, const unsigned int);

// Reveal a square, and in certain cases, spread like rabbits.
bool ms_reveal(const Game *game, const unsigned int x, const unsigned int y)
{
    if (ms_getmine(game, x, y))
        return false;

    reveal_spread(game, x, y);
    return true;
}

// The actual working end of the function above.
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

// Area of effect reveal, if you know what middle click did in old minesweeper,
// you will certainly understand what this does.
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

// Check if we've won the game yet.
bool ms_check_win(const Game *game)
{
    if (game->mines != game->flags)
        return false;

    int x, y;
    for (x = 0; x < game->sizex; x++)
        for (y = 0; y < game->sizey; y++)
            if (ms_getmine(game, x, y) != ms_getflag(game, x, y))
                return false;

    return true;
}
