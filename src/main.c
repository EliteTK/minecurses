// TODO: Clean this mess up.
#include "minesweeper.h"
#include <ncurses.h>
#include <stdlib.h>
#include <math.h>

#define MINE_DENSITY 0.15

typedef enum {
    N1 = 1, // COLOR_BLUE
    N2, // COLOR_GREEN
    N3, // COLOR_RED
    N4, // COLOR_BLUE
    N5, // COLOR_RED
    N6, // COLOR_CYAN
    N7, // COLOR_BLACK
    N8, // COLOR_BLACK

    VISIBLE, // COLOR_WHITE
    HIDDEN, // COLOR_BLACK

    FLAG, // COLOR_RED
    QUERY, // COLOR_BLACK

    MINE, // COLOR_BLACK
    NMINE, // COLOR_RED
} Colorpairs;

static bool failed = false;
static Game *game;

static inline void put_square(const Colorpairs cp, char c, bool bold)
{
    attron(COLOR_PAIR(cp) | (bold * A_BOLD));
    addch(c);
    attroff(COLOR_PAIR(cp) | (bold * A_BOLD));
}

static void draw_board()
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
                            if (!ms_xy_resolve(game, cx, cy)->visible && !ms_xy_resolve(game, cx, cy)->flag)
                                failed = !ms_reveal(game, cx, cy);
                        } else {
                            ms_genmap(game, cx, cy);
                        }
                        draw_board();
                    } else if (event.bstate & BUTTON1_DOUBLE_CLICKED) {
                        if (game->generated) {
                            failed = !ms_reveal_aoe(game, cx, cy);
                            draw_board();
                        }
                    } else if (event.bstate & BUTTON3_CLICKED) {
                        if (game->generated && !ms_xy_resolve(game, cx, cy)->visible) {
                            ms_flag_toggle(game, cx, cy);
                            draw_board();
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
                    if (!ms_xy_resolve(game, cx, cy)->visible)
                        failed = !ms_reveal(game, cx, cy);
                } else {
                    ms_genmap(game, cx, cy);
                }
                draw_board();
                break;

            case 'R': // AOE Reveal.
                if (game->generated) {
                    failed = !ms_reveal_aoe(game, cx, cy);
                    draw_board();
                }
                break;

            case 'f': // Flag
                if (game->generated && !ms_xy_resolve(game, cx, cy)->visible) {
                    ms_flag_toggle(game, cx, cy);
                    draw_board();
                }
                break;

            case 'q': // Quit
                return;
        }
        move(cx, cy);
        if (failed) return;
    }
}

int main(int argc, char **argv)
{
    initscr();
    clear();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    mousemask(BUTTON1_CLICKED | BUTTON1_DOUBLE_CLICKED | BUTTON3_CLICKED, NULL);
    start_color();

    init_pair(N1, COLOR_BLUE/* + 8*/, COLOR_WHITE);
    init_pair(N2, COLOR_GREEN, COLOR_WHITE);
    init_pair(N3, COLOR_RED, COLOR_WHITE);
    init_pair(N4, COLOR_BLUE, COLOR_WHITE);
    init_pair(N5, COLOR_MAGENTA, COLOR_WHITE);
    init_pair(N6, COLOR_CYAN, COLOR_WHITE);
    init_pair(N7, COLOR_BLACK, COLOR_WHITE);
    init_pair(N8, COLOR_BLACK/* + 8*/, COLOR_WHITE);

    init_pair(VISIBLE, COLOR_WHITE, COLOR_WHITE);
    init_pair(HIDDEN, COLOR_BLACK/* + 8*/, COLOR_BLACK/* + 8*/);

    init_pair(FLAG, COLOR_RED/* + 8*/, COLOR_BLACK/* + 8*/);
    init_pair(QUERY, COLOR_BLACK, COLOR_BLACK/* + 8*/);

    init_pair(MINE, COLOR_BLACK, COLOR_BLACK/* + 8*/);
    init_pair(NMINE, COLOR_YELLOW/* + 8*/, COLOR_BLACK/* + 8*/);

    int width, height;
    getmaxyx(stdscr, width, height);

    game = ms_newgame(width, height, width * height * MINE_DENSITY);

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
