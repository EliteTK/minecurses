#ifndef __minesweeper_h
#include <stdbool.h>

typedef struct {
    unsigned char value : 4;
    bool query : 1;
    bool flag : 1;
    bool visible : 1;
    bool mine : 1;
} Square;

typedef struct {
    Square *grid;
    unsigned int sizex;
    unsigned int sizey;
    unsigned int mines;
    unsigned int flags;
} Game;

#define __minesweeper_h
#endif
