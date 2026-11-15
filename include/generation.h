#ifndef GENERATION_H
#define GENERATION_H

#include "maze.h"

void generateMaze(maze_t *maze);
void mazePath(maze_t *maze);
void generateItems(maze_t *maze);
void removeWalls(maze_t *maze);
void normalizeMaze(maze_t *maze);

#endif