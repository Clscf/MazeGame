#ifndef FICHIER_H
#define FICHIER_H

#include "maze.h"
#include "jeu.h"

char **replaceMaze(const maze_t *maze);
maze_t *loadMaze(void);
void fileWrite(const maze_t *maze, char **newMaze);
void showScores(const maze_t *maze);

#endif