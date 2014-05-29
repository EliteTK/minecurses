#include "minesweeper.h"
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define WITHIN_BOUNDS(g, x, y) ((0 <= (x) && (x) < (g->sizex)) && (0 <= (y) && (y) < (g->sizey)))

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
    return game;
}

void ms_delgame(Game *game)
{
    free(game->grid);
    free(game);
}

unsigned char calc_value(const Game *game, const unsigned int x, const unsigned int y)
{
    // Calculate total value of mines in 3 * 3 area around (x, y).
    int dx, dy, value = 0;
    for (dx = -1; dx <= 1; dx++)
        for (dy = -1; dy <= 1; dy++)
            if (WITHIN_BOUNDS(game, x + dx, y + dy) && ms_getmine(game, x + dx, y + dy))
                value++;
    return value;
}

void ms_genmap(const Game *game, const unsigned int startx, const unsigned int starty)
{
    // Clear minefield.
    int x, y;
    for (x = 0; x < game->sizex; x++)
        for (y = 0; y < game->sizey; y++)
            ms_setmine(game, x, y, false);

    // Generate minefield.
    srand(time(NULL));
    int mines_placed = 0;
    while (mines_placed <= game->mines) {
        int x = (int)((double)rand() / ((double)RAND_MAX + 1.0) * (double)game->sizex);
        int y = (int)((double)rand() / ((double)RAND_MAX + 1.0) * (double)game->sizey);
        if (ms_getmine(game, x, y) || ((startx - 1 <= x && x <= startx + 1)
                                   && (starty - 1 <= y && y <= starty + 1)))
            continue;
        ms_setmine(game, x, y, true);
        mines_placed++;
    }

    // Calculate the "value" for each point on the map.
    for (x = 0; x < game->sizex; x++)
        for (y = 0; y < game->sizey; y++)
            ms_setvalue(game, x, y, calc_value(game, x, y));
}

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

void ms_setflag(const Game *game, const unsigned int x, const unsigned int y, const bool flag)
{
    ms_getsquare(game, x, y)->flag = flag;
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

void reveal_spread(const Game *game, const unsigned int x, const unsigned int y)
{
    // Don't propagate if the current tile is already revealed.
    if (ms_getvisible(game, x, y))
        return;

    // Make current tile visible.
    ms_setvisible(game, x, y, true);

    // Don't propagate if the current tile has a value > 0.
    if (ms_getvalue(game, x, y))
        return;

    // Spread to all 8 adjacent tiles.
    int dx, dy;
    for (dx = -1; dx <= 1; dx++)
        for (dy = -1; dy <= 1; dy++)
            if (!((dy == dx) && (dx == 0)) && WITHIN_BOUNDS(game, x + dx, y + dy))
                reveal_spread(game, x + dx, y + dy);
}

bool ms_reveal(const Game *game, const unsigned int x, const unsigned int y)
{
    // If the current tile has a mine, return false (signal that a mine was hit).
    if (ms_getmine(game, x, y))
        return false;

    // Otherwise, spread from the current tile outwards and return a successful
    // hit of a clear tile (true).
    reveal_spread(game, x, y);
    return true;
}
