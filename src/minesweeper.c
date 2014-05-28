#include "minesweeper.h"
#include <stdbool.h>
#include <stdlib.h>

Game *ms_newgame(const unsigned int sizex, const unsigned int sizey, const unsigned int mine_total)
{
    Game *game = malloc(sizeof(Game));
    game->grid = malloc(sizeof(Square)*sizex*sizey);
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

ms_genmap(const Game *game, const unsigned int startx, const unsigned int starty)
{

}

/////////////
// Getters //
/////////////

Square *ms_getsquare(const Game *game, const unsigned int x, const unsigned int y)
{
    return &(game->grid[y * game->sizex + x]);
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

bool ms_getvalue(const Game *game, const unsigned int x, const unsigned int y)
{
    return ms_getsquare(game, x, y)->value;
}

/////////////
// Setters //
/////////////

void ms_setsquare(const Game *game, const unsigned int x, const unsigned int y, Square *square)
{
    game->grid[y * game->sizex + x] = *square;
}

void ms_setmine(const Game *game, const unsigned int x, const unsigned int y, bool mine)
{
    ms_getsquare(game, x, y)->mine = mine;
}

void ms_setvisible(const Game *game, const unsigned int x, const unsigned int y, bool mine)
{
    ms_getsquare(game, x, y)->visible = mine;
}

void ms_setflag(const Game *game, const unsigned int x, const unsigned int y, bool flag)
{
    ms_getsquare(game, x, y)->flag = flag;
}

void ms_setquery(const Game *game, const unsigned int x, const unsigned int y, bool query)
{
    ms_getsquare(game, x, y)->query = query;
}

void ms_setvalue(const Game *game, const unsigned int x, const unsigned int y, unsigned char value)
{
    ms_getsquare(game, x, y)->value = value;
}

////////////////////////
// Main Functionality //
////////////////////////

void reveal_spread(const Game *game, const unsigned int x, const unsigned int y)
{
    if (ms_getvisible(game, x, y))
        return;

    ms_setvisible(game, x, y, true);

    if (ms_getvalue(game, x, y))
        return;

    int dx, dy;
    for (dx = -1; dx <= 1; dx++)
        for (dy = -1; dy <= 1; dy++)
            if (!(dy == dx == 0))
                reveal_spread(game, x + dx, y + dy);
}

bool ms_reveal(const Game *game, const unsigned int x, const unsigned int y)
{
    if (ms_getmine(game, x, y))
        return false;
    reveal_spread(game, x, y);
    return true;
}
