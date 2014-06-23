// TODO: Clean this mess up.
#include "minesweeper.h"
#include "graphics.h"
#include <ncurses.h>
#include <stdlib.h>
#include <math.h>
#include <getopt.h>

static bool failed = false;
static bool won = false;
static Game *game;

static float mine_density = 0;

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
                            if (!ms_xy_resolve(game, cx, cy)->visible
                                    && !ms_xy_resolve(game, cx, cy)->flag)
                                failed = !ms_reveal(game, cx, cy);
                        } else {
                            ms_genmap(game, cx, cy);
                        }
                        draw_board(failed);
                    } else if (event.bstate & BUTTON1_DOUBLE_CLICKED) {
                        if (game->generated) {
                            failed = !ms_reveal_aoe(game, cx, cy);
                            draw_board(failed);
                        }
                    } else if (event.bstate & BUTTON3_CLICKED) {
                        if (game->generated && !ms_xy_resolve(game, cx, cy)->visible) {
                            ms_flag_toggle(game, cx, cy);
                            draw_board(failed);
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
                draw_board(failed);
                break;

            case 'R': // AOE Reveal.
                if (game->generated) {
                    failed = !ms_reveal_aoe(game, cx, cy);
                    draw_board(failed);
                }
                break;

            case 'f': // Flag
                if (game->generated && !ms_xy_resolve(game, cx, cy)->visible) {
                    ms_flag_toggle(game, cx, cy);
                    draw_board(failed);
                }
                break;

            case 'q': // Quit
                return;
        }
        move(cx, cy);
        if (failed) return;
        if (ms_check_win(game)) {
            won = true;
            return;
        }
    }
}

// Print usage and quit.
static void print_usage(FILE *file)
{
    fputs("Usage: minecurses [options]\n\n"
          "Options:\n"
          "  -h, --help\t\tshow this help message\n"
          "  -m, --mine-density\tset density of minefield [default: 0.15]\n",
          file);
    exit(1);
}

static void getopts(int argc, char **argv)
{
    if (argc > 1) {
        int c = 0;

        while (c != -1) {
            static struct option options[] = {
                {"help",         no_argument,         0, 'h'},
                {"mine-density", required_argument,   0, 'm'},
                {0,              0,                   0, 0},
            };

            int option_index = 0;

            c = getopt_long(argc, argv, "hm:", options, &option_index);

            switch (c) {
                case 'h':
                    print_usage(stdout);
                    break;

                case 'm':
                    mine_density = strtod(optarg, NULL);
                    break;
            }
        }
    }
}

int main(int argc, char **argv)
{
    getopts(argc, argv);
    if (mine_density <= 0 || mine_density >= 1) mine_density = 0.15;

    ginit();

    int width, height;
    getmaxyx(stdscr, width, height);

    game = ms_newgame(width, height, width * height * mine_density);

    set_game(game);

    board_clear();

    run_game();

    if (failed)
        getch();

    gcleanup();

    ms_delgame(game);

    return 0;
}
