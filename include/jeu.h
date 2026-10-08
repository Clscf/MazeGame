#ifndef JEU_H
#define JEU_H

#include "maze.h"
#include "affichage.h"

void game(maze_t *maze);
int movement(char move, maze_t *maze);

#endif