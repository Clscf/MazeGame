#ifndef JEU_H
#define JEU_H

#include "maze.h"
#include "affichage.h"

#define MOVE 2
#define TRAP 100
#define TREASURE 100
#define SCORE_COUNT 10
#define PSEUDO_LENGTH 50

typedef struct {
    char pseudo[PSEUDO_LENGTH];
    int score;
} score_entry_t;

struct stats
{
    int score;
    int key;
};
typedef struct stats stats_t;


void game(maze_t *maze, stats_t *stats);
int movement(char move, maze_t *maze, stats_t *stats);
void saveScore(const maze_t *maze, const stats_t *stats);

#endif