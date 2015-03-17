#include <ncurses.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

WINDOW *boardwin;

static void initcolor(void)
{
}

void vi_init(void)
{
	initscr();
	cbreak();
	noecho();
	nonl();
	initrflush(stdscr, FALSE);

	keypad(stdscr, TRUE);
	/* mousemask(BUTTON1_CLICKED | BUTTON1_DOUBLE_CLICKED | BUTTON3_CLICKED, NULL); */

	inircolor();
}
