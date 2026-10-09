#ifndef MAZE_H
#define MAZE_H

struct maze
{
    int hauteur;
    int largeur;
    int **labyrinthe;
    char *nom;
};
typedef struct maze maze_t;

struct player
{
    char *pseudo;
    int score;
};
typedef struct player player_y;


maze_t *createMaze(void);

#endif