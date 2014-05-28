minecurses
==========

NCurses minesweeper.

TODO:
-----
1. Write a base minesweeper system which has simple io commands allowing it to get different "maps" of the current game state, and input which allows to directly change any piece of the game i.e. reveal a part, flag a mine, or queryflag a mine.
2. Write a solver for this base system which then integrates with the game to dynamically intelligently create a map which is solvable without ever having to guess any piece.
3. At all stages: attempt to write a ncurses interface for all this.

End goal:
---------
A classic minesweeper game which generates fair maps, provides different levels of difficulty has a simple and powerful keyboard interface with the possibility of a mouse interface.
