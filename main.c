// TODO: Clean this mess up.
#include "minesweeper.h"
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <getopt.h>

typedef enum {
    N1 = 1,	// COLOR_BLUE
    N2,		// COLOR_GREEN
    N3,		// COLOR_RED
    N4,		// COLOR_BLUE
    N5,		// COLOR_RED
    N6,		// COLOR_CYAN
    N7,		// COLOR_BLACK
    N8,		// COLOR_BLACK

    VISIBLE,	// COLOR_WHITE
    HIDDEN,	// COLOR_BLACK

    FLAG,	// COLOR_RED
    QUERY,	// COLOR_BLACK

    MINE,	// COLOR_BLACK
    NMINE	// COLOR_RED
} Colorpairs;

static bool failed = false;
static Game *game;

static bool hi_vis = false;

static inline void put_square(const Colorpairs cp, char c, bool bold)
{
    attron(COLOR_PAIR(cp) | (bold * A_BOLD));
    addch(c);
    attroff(COLOR_PAIR(cp) | (bold * A_BOLD));
}

static void draw_board(int cx, int cy)
{
    int x, y;
    for (x = 0; x < game->sizex; x++)
        for (y = 0; y < game->sizey; y++) {
            move(x, y);

            if (!ms_getvisible(game, x, y)) {
                if (failed && ms_getmine(game, x, y) && !ms_getflag(game, x, y)) {
                    put_square(MINE, '*', true);
                    continue;
                }

                if (ms_getflag(game, x, y)) {
                    if (failed && !ms_getmine(game, x, y)) {
                        put_square(NMINE, '*', true);
                        continue;
                    }

                    put_square(FLAG, 'X', true);
                    continue;
                }

                if (ms_getquery(game, x, y)) {
                    put_square(QUERY, '?', true);
                    continue;
                }

                if (cx == x && cy == y && hi_vis)
                    put_square(HIDDEN, '#', false);
                else
                    put_square(HIDDEN, ' ', false);
                continue;
            }

            if (!ms_getvalue(game, x, y)) {
                if (cx == x && cy == y && hi_vis)
                    put_square(VISIBLE, '#', false);
                else
                    put_square(VISIBLE, ' ', false);
                continue;
            }

            unsigned char value = ms_getvalue(game, x, y);
            put_square(value, value + '0', false);
        }
    refresh();
}

static void run_game()
{
    MEVENT event;

    int c;

    int cx = game->sizex / 2, cy = game->sizey / 2;
    move(cx, cy);

    while (!failed) {
        c = getch();
        switch (c) {
        // Mouse input.
        case KEY_MOUSE:
            if (getmouse(&event) == OK) {
                cx = event.y; // No, I don't know why.
                cy = event.x; // Your guess is as good as mine.
                if (event.bstate & BUTTON1_CLICKED) {
                    if (game->generated) {
                        if (!ms_getvisible(game, cx, cy) && !ms_getflag(game, cx, cy))
                            failed = !ms_reveal(game, cx, cy);
                    } else {
                        ms_genmap(game, cx, cy);
                    }
                    draw_board(cx, cy);
                } else if (event.bstate & BUTTON1_DOUBLE_CLICKED) {
                    if (game->generated) {
                        failed = !ms_reveal_aoe(game, cx, cy);
                        draw_board(cx, cy);
                    }
                } else if (event.bstate & BUTTON3_CLICKED) {
                    if (game->generated && !ms_getvisible(game, cx, cy)) {
                        ms_setflag(game, cx, cy, !ms_getflag(game, cx, cy));
                        draw_board(cx, cy);
                    }
                }
            }

            break;

            // Cursor motion.
        case 'w': case 'k':
            if (cx > 0) cx--; break;
        case 'W': case 'K':
            if (cx > 4) cx -= 5; break;

        case 's': case 'j':
            if (cx < (game->sizex - 1)) cx++; break;
        case 'S': case 'J':
            if (cx < (game->sizex - 5)) cx += 5; break;

        case 'a': case 'h':
            if (cy > 0) cy--; break;
        case 'A': case 'H':
            if (cy > 4) cy -= 5; break;

        case 'd': case 'l':
            if (cy < (game->sizey - 1)) cy++; break;
        case 'D': case 'L':
            if (cy < (game->sizey - 5)) cy += 5; break;


        case 'r': // Reveal.
            if (game->generated) {
                if (!ms_getvisible(game, cx, cy))
                    failed = !ms_reveal(game, cx, cy);
            } else {
                ms_genmap(game, cx, cy);
            }
            /*draw_board();*/
            break;

        case 'R': // AOE Reveal.
            if (game->generated) {
                failed = !ms_reveal_aoe(game, cx, cy);
                /*draw_board();*/
            }
            break;

        case 'f': // Flag
            if (game->generated && !ms_getvisible(game, cx, cy)) {
                ms_setflag(game, cx, cy, !ms_getflag(game, cx, cy));
                /*draw_board();*/
            }
            break;

        case 'q': // Quit
            return;
        }

        draw_board(cx, cy);
        move(cx, cy);
        if (failed) return;
    }
}

void usage()
{
    fputs("Usage: minecurses [options]\n\n"
          "Options:\n"
          "  -h, --help		show this help message\n"
          "  -m, --mine-density	set density of minefield [default: 0.15]\n"
          "  -c, --cursor	turn on character cursor mode (uses # as cursor)\n",
          stderr);
    exit(1);
}

int main(int argc, char **argv)
{
    double mine_density = 0;

    if (argc > 1) {
        int c;

        while (c != -1) {
            static struct option options[] = {
                // Flags
                {"help",		no_argument,		0, 'h'},
                {"cursor",		no_argument,		0, 'c'},
                // Switches
                {"mine-density",	required_argument,	0, 'm'},
                {0,              0,                   0, 0},
            };

            int option_index = 0;

            c = getopt_long(argc, argv, "hcm:", options, &option_index);

            switch (c) {
                case 'h':
                    usage();
                    break;

                case 'c':
                    hi_vis = true;
                    break;

                case 'm':
                    mine_density = strtod(optarg, NULL);
                    break;
            }
        }
    }

    if (mine_density <= 0 || mine_density >= 1) mine_density = 0.15;

    initscr();
    clear();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    mousemask(BUTTON1_CLICKED | BUTTON1_DOUBLE_CLICKED | BUTTON3_CLICKED, NULL);
    start_color();

    init_pair(N1,	COLOR_BLUE,	COLOR_WHITE);
    init_pair(N2,	COLOR_GREEN,	COLOR_WHITE);
    init_pair(N3,	COLOR_RED,	COLOR_WHITE);
    init_pair(N4,	COLOR_BLUE,	COLOR_WHITE);
    init_pair(N5,	COLOR_MAGENTA,	COLOR_WHITE);
    init_pair(N6,	COLOR_CYAN,	COLOR_WHITE);
    init_pair(N7,	COLOR_BLACK,	COLOR_WHITE);
    init_pair(N8,	COLOR_BLACK,	COLOR_WHITE);

    init_pair(VISIBLE,	COLOR_BLACK,	COLOR_WHITE);
    init_pair(HIDDEN,	COLOR_WHITE,	COLOR_BLACK);

    init_pair(FLAG,	COLOR_RED,	COLOR_BLACK);
    init_pair(QUERY,	COLOR_BLACK,	COLOR_BLACK); /* Not used. */

    init_pair(MINE,	COLOR_WHITE,	COLOR_BLACK);
    init_pair(NMINE,	COLOR_YELLOW,	COLOR_BLACK);

    int width, height;
    getmaxyx(stdscr, width, height);

    game = ms_newgame(width, height, width * height * mine_density);

    int x, y;
    attron(COLOR_PAIR(HIDDEN));
    for (x = 0; x < game->sizex; x++)
        for (y = 0; y < game->sizey; y++)
            addch(' ');
    attroff(COLOR_PAIR(HIDDEN));

    run_game();

    if (failed)
        getch();

    ms_delgame(game);
    endwin();

    return 0;
}
