#ifndef MONSTRE_H
#define MONSTRE_H

#include "maze.h"

#define PENALTY_PER_RANGE 10

typedef struct monster monster_t;

typedef void (*deplacement_t)(monster_t *monstre, const maze_t *maze,
                              int joueurX, int joueurY);

typedef struct {
    const char *nom;
    char symbole;
    int penalite;
    deplacement_t deplacer;
    const char *description;
} monster_type_t;

struct monster {
    const monster_type_t *type;
    int x, y;
    int departX, departY;
    int centreX, centreY;
    int etendue;
    int garde;
};

void generateMonsters(maze_t *maze);
void loadMonsters(maze_t *maze);
void resetMonsters(maze_t *maze);
void moveMonsters(maze_t *maze, int joueurX, int joueurY);
int monsterCollisions(maze_t *maze, int joueurX, int joueurY);
const monster_t *monsterAt(const maze_t *maze, int x, int y);
void showMonsterLegend(const maze_t *maze);

#endif
