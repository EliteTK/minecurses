#include "minesweeper.h"
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

char get_correct_char(const Game *game, const unsigned int x, const unsigned int y)
{
    if (ms_getmine(game, x, y))
        return '*';
    if (!ms_getvalue(game, x, y))
        return '.';
    return ms_getvalue(game, x, y) + '0';
}

int main(int argc, char **argv)
{
    initscr();

    int width, height;
    getmaxyx(stdscr, width, height);

    Game *game = ms_newgame(width, height, width * height * 0.1);

    ms_genmap(game, 10, 10);
    getch();

    int x, y;
    for (x = 0; x < width; x++)
        for (y = 0; y < height; y++)
            mvaddch(x, y, get_correct_char(game, x, y));
    refresh();

    getch();

    ms_delgame(game);
    endwin();
}
