#ifndef FICHIER_H
#define FICHIER_H

#include "maze.h"

char **replaceMaze(const maze_t *maze);
maze_t *loadMaze(void);
void fileWrite(const maze_t *maze, char **newMaze);
static void freeMazeText(char **mazeText, int hauteur);

#endif