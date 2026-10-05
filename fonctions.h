#include <stdio.h>
#include <stdlib.h>

struct maze
{
    int hauteur;
    int largeur;
    int **labyrinthe;
    char *nom;
};
typedef struct maze maze_t;

void displayMaze(maze_t maze);
void generateMaze(maze_t maze);
void mazePath(maze_t maze);
void menu();
void createMaze();
