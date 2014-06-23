#ifndef __GRAPHICS_H

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

bool ginit();
bool gcleanup();
void board_clear();
void set_game(Game *game);
void draw_menu();
void draw_status();
void draw_board(bool failed);

#define __GRAPHICS_H 1
#endif
