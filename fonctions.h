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
int menu();
maze_t createMaze();
void fileWrite(maze_t maze, char** newMaze);
char** replaceMaze(maze_t maze);
maze_t loadMaze();
void game(maze_t maze);
int movement(char move, maze_t maze);

