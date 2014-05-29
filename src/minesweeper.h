#ifndef __minesweeper_h
#include <stdbool.h>

typedef struct {
    unsigned char
        query : 1,
        flag : 1,
        visible : 1,
        mine : 1,
        value : 4;
} Square;

typedef struct {
    Square **grid;
    unsigned int sizex;
    unsigned int sizey;
    unsigned int mines;
    unsigned int flags;
    bool generated;
} Game;

Game *ms_newgame(const unsigned int, const unsigned int, const unsigned int);
void ms_delgame(Game *);
void ms_genmap(const Game *game, const unsigned int startx, const unsigned int starty);
Square *ms_getsquare(const Game *, const unsigned int, const unsigned int);
bool ms_getmine(const Game *, const unsigned int, const unsigned int);
bool ms_getvisible(const Game *, const unsigned int, const unsigned int);
bool ms_getflag(const Game *, const unsigned int, const unsigned int);
bool ms_getquery(const Game *, const unsigned int, const unsigned int);
unsigned char ms_getvalue(const Game *, const unsigned int, const unsigned int);
void ms_setsquare(const Game *, const unsigned int, const unsigned int, Square *);
void ms_setmine(const Game *, const unsigned int, const unsigned int, const bool);
void ms_setvisible(const Game *, const unsigned int, const unsigned int, const bool);
void ms_setflag(const Game *, const unsigned int, const unsigned int, const bool);
void ms_setquery(const Game *, const unsigned int, const unsigned int, const bool);
void ms_setvalue(const Game *, const unsigned int, const unsigned int, const unsigned char);
bool ms_reveal(const Game *, const unsigned int, const unsigned int);

#define __minesweeper_h 1
#endif
