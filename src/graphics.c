#include "minesweeper.h"
#include "graphics.h"
#include <stdlib.h>



// Initialise graphics.
bool ginit()
{
    init_pair(N1, COLOR_BLUE, COLOR_WHITE);
    init_pair(N2, COLOR_GREEN, COLOR_WHITE);
    init_pair(N3, COLOR_RED, COLOR_WHITE);
    init_pair(N4, COLOR_BLUE, COLOR_WHITE);
    init_pair(N5, COLOR_MAGENTA, COLOR_WHITE);
    init_pair(N6, COLOR_CYAN, COLOR_WHITE);
    init_pair(N7, COLOR_BLACK, COLOR_WHITE);
    init_pair(N8, COLOR_BLACK, COLOR_WHITE);

    init_pair(VISIBLE, COLOR_WHITE, COLOR_WHITE);
    init_pair(HIDDEN, COLOR_BLACK, COLOR_BLACK);

    init_pair(FLAG, COLOR_RED, COLOR_BLACK);
    init_pair(QUERY, COLOR_BLACK, COLOR_BLACK);

    init_pair(MINE, COLOR_BLACK, COLOR_BLACK);
    init_pair(NMINE, COLOR_YELLOW, COLOR_BLACK);

    int x, y;
    attron(COLOR_PAIR(HIDDEN));
    for (x = 0; x < game->sizex; x++)
        for (y = 0; y < game->sizey; y++)
            addch(' ');
    attroff(COLOR_PAIR(HIDDEN));
}

// Cleanup graphics.
bool gcleanup()
{
}


static inline void put_square(const Colorpairs, char, bool);

void draw_board()
{
    int x, y;
    for (x = 0; x < game->sizex; x++)
        for (y = 0; y < game->sizey; y++) {
            move(x, y);

            if (!ms_xy_resolve(game, x, y)->visible) {
                if (failed && ms_xy_resolve(game, x, y)->mine && !ms_xy_resolve(game, x, y)->flag) {
                    put_square(MINE, '*', true);
                    continue;
                }

                if (ms_xy_resolve(game, x, y)->flag) {
                    if (failed && !ms_xy_resolve(game, x, y)->mine) {
                        put_square(NMINE, '*', true);
                        continue;
                    }

                    put_square(FLAG, 'X', true);
                    continue;
                }

                if (ms_xy_resolve(game, x, y)->query) {
                    put_square(QUERY, '?', true);
                    continue;
                }

                put_square(HIDDEN, ' ', false);
                continue;
            }

            if (!ms_xy_resolve(game, x, y)->value) {
                put_square(VISIBLE, ' ', false);
                continue;
            }

            unsigned char value = ms_xy_resolve(game, x, y)->value;
            put_square(value, value + '0', false);
        }
    refresh();
}

static inline void put_square(const Colorpairs cp, char c, bool bold)
{
    attron(COLOR_PAIR(cp) | (bold * A_BOLD));
    addch(c);
    attroff(COLOR_PAIR(cp) | (bold * A_BOLD));
}
