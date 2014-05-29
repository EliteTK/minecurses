// TODO: Clean this mess up.
#include "minesweeper.h"
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MINE_DENSITY 0.15

enum colorpairs {
    N1 = 1, // COLOR_BLUE + 8
    N2, // COLOR_GREEN
    N3, // COLOR_RED + 8
    N4, // COLOR_BLUE
    N5, // COLOR_RED
    N6, // COLOR_CYAN
    N7, // COLOR_BLACK
    N8, // COLOR_BLACK + 8

    VISIBLE, // COLOR_WHITE
    HIDDEN, // COLOR_BLACK + 8

    FLAG, // COLOR_RED
    QUERY, // COLOR_BLACK

    MINE, // COLOR_BLACK
    NMINE, // COLOR_RED + 8
};

typedef struct {
    unsigned int x;
    unsigned int y;
} Coord;

int main(int argc, char **argv)
{
    bool failed = false;
    initscr();
    raw();
    noecho();
    start_color();

    init_pair(N1, COLOR_BLUE + 8, COLOR_WHITE);
    init_pair(N2, COLOR_GREEN, COLOR_WHITE);
    init_pair(N3, COLOR_RED, COLOR_WHITE);
    init_pair(N4, COLOR_BLUE, COLOR_WHITE);
    init_pair(N5, COLOR_MAGENTA, COLOR_WHITE);
    init_pair(N6, COLOR_CYAN, COLOR_WHITE);
    init_pair(N7, COLOR_BLACK, COLOR_WHITE);
    init_pair(N8, COLOR_BLACK + 8, COLOR_WHITE);

    init_pair(VISIBLE, COLOR_WHITE, COLOR_WHITE);
    init_pair(HIDDEN, COLOR_BLACK + 8, COLOR_BLACK + 8);

    init_pair(FLAG, COLOR_RED + 8, COLOR_BLACK + 8);
    init_pair(QUERY, COLOR_BLACK, COLOR_BLACK + 8);

    init_pair(MINE, COLOR_BLACK, COLOR_BLACK + 8);
    init_pair(NMINE, COLOR_RED + 8, COLOR_BLACK + 8);

    int width, height;
    getmaxyx(stdscr, width, height);

    Game *game = ms_newgame(width, height, width * height * MINE_DENSITY);

    int x, y;
    attron(COLOR_PAIR(HIDDEN));
    for (x = 0; x < width; x++)
        for (y = 0; y < height; y++)
            addch(' ');
    attroff(COLOR_PAIR(HIDDEN));

    bool run = true, update = true;
    int cx = 0, cy = 0;
    move(cx, cy);
    while (run && !failed) {
        char c = getch();
        switch (c) {
            // Cursor motion.
            case 'w': case 'k':
                if (cx > 0) cx--; break;
            case 'W': case 'K':
                if (cx > 4) cx -= 5; break;

            case 's': case 'j':
                if (cx < (width - 1)) cx++; break;
            case 'S': case 'J':
                if (cx < (width - 5)) cx += 5; break;

            case 'a': case 'h':
                if (cy > 0) cy--; break;
            case 'A': case 'H':
                if (cy > 4) cy -= 5; break;

            case 'd': case 'l':
                if (cy < (height - 1)) cy++; break;
            case 'D': case 'L':
                if (cy < (height - 5)) cy += 5; break;

            case 'r': // Reveal.
                if (game->generated) {
                    if (!ms_getvisible(game, cx, cy))
                        failed = !ms_reveal(game, cx, cy);
                } else {
                    ms_genmap(game, cx, cy);
                }
                update = true;
                break;

            case 'R': // AOE Reveal.
                if (game->generated) {
                    failed = !ms_reveal_aoe(game, cx, cy);
                }
                update = true;
                break;

            case 'f': // Flag
                if (game->generated && !ms_getvisible(game, cx, cy))
                    ms_setflag(game, cx, cy, !ms_getflag(game, cx, cy));
                update = true;
                break;

            case 'q': // Quit
                run = false; break;
        }

        if (update) {
            for (x = 0; x < width; x++)
                for (y = 0; y < height; y++) {
                    move(x, y);

                    if (!ms_getvisible(game, x, y)) {
                        if (failed && ms_getmine(game, x, y) && !ms_getflag(game, x, y)) {
                            attron(COLOR_PAIR(MINE));
                            addch('*');
                            attroff(COLOR_PAIR(MINE));
                            continue;
                        }

                        if (ms_getflag(game, x, y)) {
                            if (failed && !ms_getmine(game, x, y)) {
                                attron(COLOR_PAIR(NMINE));
                                addch('*');
                                attroff(COLOR_PAIR(NMINE));
                                continue;
                            }

                            attron(COLOR_PAIR(FLAG));
                            addch('X');
                            attroff(COLOR_PAIR(FLAG));
                            continue;
                        }

                        if (ms_getquery(game, x, y)) {
                            attron(COLOR_PAIR(QUERY));
                            addch('?');
                            attroff(COLOR_PAIR(QUERY));
                            continue;
                        }

                        attron(COLOR_PAIR(HIDDEN));
                        addch(' ');
                        attroff(COLOR_PAIR(HIDDEN));
                        continue;
                    }

                    if (!ms_getvalue(game, x, y)) {
                        attron(COLOR_PAIR(VISIBLE));
                        addch(' ');
                        attroff(COLOR_PAIR(VISIBLE));
                        continue;
                    }

                    unsigned char value = ms_getvalue(game, x, y);
                    attron(COLOR_PAIR(value));
                    addch(value + '0');
                    attroff(COLOR_PAIR(value));
                }
            refresh();
            update = false;
        }
        move(cx, cy);
    }

    if (failed)
        getch();

    ms_delgame(game);
    endwin();

    return 0;
}
