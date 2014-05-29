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

Game *ms_new(const unsigned int, const unsigned int, const unsigned int);
void ms_del(Game *);
Square *ms_getsquare(const Game *, const unsigned int, const unsigned int);
bool ms_getmine(const Game *, const unsigned int, const unsigned int);
bool ms_getvisible(const Game *, const unsigned int, const unsigned int);
bool ms_getflag(const Game *, const unsigned int, const unsigned int);
bool ms_getquer(const Game *, const unsigned int, const unsigned int);
bool ms_getvalue(const Game *, const unsigned int, const unsigned int);
void ms_setsquare(const Game *, const unsigned int, const unsigned int, Square *);
void ms_setmine(const Game *, const unsigned int, const unsigned int, bool);
void ms_setvisible(const Game *, const unsigned int, const unsigned int, bool);
void ms_setflag(const Game *, const unsigned int, const unsigned int, bool);
void ms_setquer(const Game *, const unsigned int, const unsigned int y, bool);
void ms_setvalue(const Game *, const unsigned int, const unsigned int, unsigned char);
bool ms_reveal(const Game *, const unsigned int, const unsigned int);

#define __minesweeper_h
#endif
